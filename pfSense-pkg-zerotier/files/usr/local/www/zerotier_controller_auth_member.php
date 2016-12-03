<?php
require_once("guiconfig.inc");
require_once("zerotier.inc");

# XXX: Add check if service is running.

if ($_POST && 
	ctype_xdigit($_POST['memid']) && strlen($_POST['memid']) == 10 &&
	ctype_xdigit($_POST['net']) && strlen($_POST['net']) == 16) {

	$member = json_decode(sendPost("GET", "/controller/network/${_POST['net']}/member/${_POST['memid']}", null));

	if ($_POST['mode'] == 'auth'){
		if ($member->authorized)
			$res = json_decode(sendPost("POST", "/controller/network/${_POST['net']}/member/${_POST['memid']}", json_encode(['authorized'=>false])));
		if (!$member->authorized)
			$res = json_decode(sendPost("POST", "/controller/network/${_POST['net']}/member/${_POST['memid']}", json_encode(['authorized'=>true])));
		echo $res->authorized == true ? 'yes' : 'no';

	} else if ($_POST['mode'] == 'bridge'){
		if ($member->activeBridge)
			$res = json_decode(sendPost("POST", "/controller/network/${_POST['net']}/member/${_POST['memid']}", json_encode(['activeBridge'=>false])));
		if (!$member->activeBridge)
			$res = json_decode(sendPost("POST", "/controller/network/${_POST['net']}/member/${_POST['memid']}", json_encode(['activeBridge'=>true])));
		echo $res->activeBridge == true ? 'yes' : 'no';	
	}
	exit;
}

$pgtitle = array(gettext("Package"), gettext("ZeroTier"), gettext("Network Members"));
require("head.inc");

$tab_array = array();
$tab_array[] = array(gettext("Client"), false, "pkg.php?xml=zerotier.xml");
$tab_array[] = array(gettext("Client Status"), false, "zerotier_status.php");
$tab_array[] = array(gettext("Controller"), false, "pkg.php?xml=zerotiercontroller.xml");
$tab_array[] = array(gettext("Network Members"), true, "zerotier_controller_auth_member.php");
display_top_tabs($tab_array);
?>
<script>
function flip(m, i, n) {
    var http = new XMLHttpRequest();
    http.open("POST", "", true);
    http.setRequestHeader("Content-type","application/x-www-form-urlencoded");
    http.send("mode="+m+"&memid="+i+"&net="+n);
    http.onload = function() {
        if(http.responseText == 'yes'){
        	document.getElementById(n+'-'+m+'-'+i).innerHTML = 'yes';
        } else if(http.responseText == 'no'){
        	document.getElementById(n+'-'+m+'-'+i).innerHTML = 'no';
        }
    }
}
</script>
<div class="panel panel-default">
	<div class="panel-heading"><h2 class="panel-title">ZeroTier Network Members</h2></div>
	<div class="panel-body">
	<div class="table-responsive">
	<table class="table table-striped table-hover table-condensed">
		<colgroup>
		   <col span="1" style="width: 10%;">
		   <col span="1" style="width: 10%;">
		   <col span="1" style="width: 10%;">
		   <col span="1" style="width: 10%;">
		   <col span="1" style="width: 10%;">
		</colgroup>
		<thead>
			<tr>
				<th>Network ID</th>
				<th>Authorized?</th>
				<th>Address</th>
				<th>Assigned IP</th>
				<th>Bridge?</th>
			</tr>
		</thead>
		<tbody>	<?php
			
			// Get list of networks from controller
			$listn = json_decode(sendPost("GET", "/controller/network", null));
			foreach ($listn as $nets):
			?>
				<tr>
					<th>Net ID: <?= $nets?></th>
					<th /><th /><th /><th />
				</tr>
				<?php
				// Get list of members on net from controller
				$listm = json_decode(sendPost("GET", "/controller/network/${nets}/member", null));
				foreach (array_keys(get_object_vars($listm)) as $member):

					// Get member details from controller
					$m = json_decode(sendPost("GET", "/controller/network/${nets}/member/${member}", null));
				?>
				<tr>
					<td />
					<td><a id='<?= $nets?>-auth-<?= $m->address?>' onclick="flip('auth','<?= $m->address?>','<?= $nets?>')"><?= $m->authorized ? 'yes' : 'no' ?></a></td>
					<td><?= $m->address?></td>
					<td><?= implode(',',$m->ipAssignments)?></td>
					<td><a id='<?= $nets?>-bridge-<?= $m->address?>' onclick="flip('bridge','<?= $m->address?>','<?= $nets?>')"><?= $m->activeBridge ? 'yes' : 'no' ?></a></td>
				</tr>
				<? endforeach ?>
			<? endforeach ?></tbody>
	</table>
	</div>
</div>
<?php include("foot.inc"); ?>