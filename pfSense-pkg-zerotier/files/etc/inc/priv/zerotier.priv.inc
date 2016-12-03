<?php
global $priv_list;

$priv_list['page-vpn-zerotier-client'] = array();
$priv_list['page-vpn-zerotier-client']['name'] = "WebCfg - vpn: ZeroTier client";
$priv_list['page-vpn-zerotier-client']['descr'] = "Allow access to ZeroTier client GUI";
$priv_list['page-vpn-zerotier-client']['match'] = array();
$priv_list['page-vpn-zerotier-client']['match'][] = "pkg_edit.php?xml=zerotier.xml*";
$priv_list['page-vpn-zerotier-client']['match'][] = "zerotier_status.php*";

$priv_list['page-vpn-zerotier-controlle:'] = array();
$priv_list['page-vpn-zerotier-controller']['name'] = "WebCfg - vpn: ZeroTier controller";
$priv_list['page-vpn-zerotier-controller']['descr'] = "Allow access to ZeroTier controller GUI";
$priv_list['page-vpn-zerotier-controller']['match'] = array();
$priv_list['page-vpn-zerotier-controller']['match'][] = "pkg_edit.php?xml=zerotiercontroller.xml*";
$priv_list['page-vpn-zerotier-controller']['match'][] = "zerotier_controller_auth_member.php*";
