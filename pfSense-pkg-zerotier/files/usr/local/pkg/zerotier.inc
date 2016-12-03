<?php
require_once('config.inc');
require_once('service-utils.inc');
require_once('util.inc');

function zerotier_install() {
 	global $config;
	$rc['file'] = 'zerotier-one.sh';
	$rc['start'] .= "/usr/local/sbin/zerotier-one -d";
	$rc['stop'] .= "/usr/bin/killall zerotier-one";
	write_rcfile($rc);
 	write_config("[zerotier] Package installed.");
 }

# Used by Status Page
function zerotier_list_joined_networks() {
    exec("/usr/local/bin/zerotier-cli -j listnetworks", $netj);
    $nets = json_decode(implode('',$netj));
    return $nets;
}

function zerotier_join_network(){
	# When joining I know the network ID
    system("/usr/local/bin/zerotier-cli join {$_POST['joinnetwork']}");
}

function zerotier_leave_network(){
	global $config;
	
    # Get list of Network IDs in JSON
    exec("/usr/local/bin/zerotier-cli -j listnetworks", $netj);
    $nets = json_decode(implode('',$netj));
    foreach ($nets as $net){
        # Check if a joined network is in config
        if (!in_array($net->nwid, array_column($config['installedpackages']['zerotierclient']['config'], 'joinnetwork'))){
            # If it's not in config then leave
            exec("/usr/local/bin/zerotier-cli leave {$net->nwid}"); 
        }
    }
}

function validate_form_zerotier($post, &$input_errors) {
    if ($post['joinnetwork'] == '' || ($post['joinnetwork'] && (!ctype_xdigit($post['joinnetwork']) || (strlen($post['joinnetwork']) <> 16)))) {
        $input_errors[] = '"Network ID" must be 16 characters long and contain only A-F, 0-9';
    }
}

function validate_form_zerotier_controller($post, &$input_errors) {
    if ($post['network'] && (!ctype_xdigit($post['network']) || (strlen($post['network']) <> 16)) ) {
        $input_errors[] = '"Network ID" can be blank or 16 characters long and contain only A-F, 0-9';
    }
	if (!$post['allowallprotocols'] && !$post['allowipv4'] && !$post['allowipv6'] ) {
        $input_errors[] = 'You have not Allowed any protocols.';
    }
}

function zerotier_controller_sync(){
	zerotier_controller_sync_delete();
	zerotier_controller_sync_create();
}

function zerotier_controller_sync_create(){
//     global $config;

// 	foreach ($config['installedpackages']['zerotiercontroller']['config'] as $net){
// // 		echo var_dump($net);
// 	}
	zerotier_create_network($_POST);	
}

function zerotier_controller_sync_delete(){
    global $config;

	// Get list of networks from controller
	$networks = sendPost("GET", "/controller/network", null);

    $nets = json_decode($networks);
    foreach ($nets as $net){
        # Check if a created network is in config
        if (!in_array($net, array_column($config['installedpackages']['zerotiercontroller']['config'], 'network'))){
            # If it's not in config then delete it
			$res = sendPost("DELETE", "/controller/network/${net}", null);
		}
    }
}

function zerotier_create_network($post){
    global $config;
    global $id;
	
    // Use a random network id if one is not provided
	if ($post['network'] !== '' ){
		$network = $post['network'];
	} else {
		// 	Get controller address
		$address = json_decode(sendPost("GET", "/status", null));
		$address = $address->address;
		$network = "${address}______";
	}
	
	// Use it to append to networkid to create network
	$created = json_decode(sendPost("POST", "/controller/network/${network}", json_encode(zerotier_convert_post_to_object($post))));   
	
	// if network was blank, record new network ID
	if (isset($id) && $config['installedpackages']['zerotiercontroller']['config'][$id]['network'] == '') {
		$config['installedpackages']['zerotiercontroller']['config'][$id]['network'] = $created->nwid;
		write_config();
	}
}

function zerotier_convert_post_to_object($p){
    //  String
    $o['name']          = $p['name'];
    $o['v4AssignMode']  = $p['v4assignmode'] == 'on' ? 'zt' : '';
    $o['v6AssignMode']  = $p['v6assignmode']; // group_concat

    //  Boolean
    $o['private']               = $p['private'] == 'on' ? true : false;
    $o['enableBroadcast']       = $p['enablebroadcast'] == 'on' ? true : false;
    $o['allowPassiveBridging']  = $p['allowpassivebridging'] == 'on' ? true : false;
    
    //  Integer
    $o['multicastLimit']    = (integer) $p['multicastlimit'];

	$parsedRelays = [];
	$parsedRoutes = [];
	$parsedRules = [];
	$parsedPools = [];

	// Routes
	if (isset($p['routes'])){
		$parsedRoutes = explode(',',$p['routes']);	
	}

	// Rules
	if (isset($p['allowallprotocols'])){
		$parsedRules[] = ['ruleNo' => 1, 'action' => 'accept'];
	} else {
		if (isset($p['allowipv4'])){
			$parsedRules[] = ['ruleNo' => 1, 'etherType' => 2048, 'action' => 'accept'];
			$parsedRules[] = ['ruleNo' => 2, 'etherType' => 2054, 'action' => 'accept'];
		}
		if (isset($p['allowipv6'])){
			$parsedRules[] = ['ruleNo' => 3, 'etherType' => 34525, 'action' => 'accept'];
		}
	}

	// IP Assignment Poools v4 and v6
	if ($p['v4assignmode'] == 'on' && isset($p['ipv4rangestart']) && isset($p['ipv4rangeend'])){
		$parsedPools[] = ['ipRangeStart' => $p['ipv4rangestart'],
						  'ipRangeEnd' => $p['ipv4rangeend']];
	}
	if (in_array('zt', $p['v6assignmode']) && isset($p['ipv6rangestart']) && isset($p['ipv6rangeend'])){
		$parsedPools[] = ['ipRangeStart' => $p['ipv6rangestart'],
						  'ipRangeEnd' => $p['ipv6rangeend']];
	}

	// Arrays				  
    $o['relays']            = $parsedRelays;
//     $o['routes']            = $parsedRoutes;   # This line can be used when zerotier is upgraded
    $o['ipLocalRoutes']     = $parsedRoutes;
    $o['rules']             = $parsedRules;
    $o['ipAssignmentPools'] = $parsedPools;
    
    return $o;
}

function sendPost($req, $url, $post){
  $API_KEY = file_get_contents('/var/db/zerotier-one/authtoken.secret');
  $url = 'http://localhost:9993' . $url;
  
  $c = curl_init($url);
  curl_setopt($c, CURLOPT_CUSTOMREQUEST, $req);  
  curl_setopt($c, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($c, CURLOPT_POSTFIELDS, $post);
  curl_setopt($c, CURLOPT_FOLLOWLOCATION, 1);
  curl_setopt($c, CURLOPT_HTTPHEADER, array(
        "X-ZT1-Auth: $API_KEY"
        ));
 
  $result = curl_exec($c);
  curl_close($c);
  return $result;
}
