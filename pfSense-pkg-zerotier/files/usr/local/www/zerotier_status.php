<?php
require_once("guiconfig.inc");
require_once("zerotier.inc");

# XXX: Add check if service is running.

$pgtitle = array(gettext("Package"), gettext("ZeroTier"), gettext("Client Status"));
require("head.inc");

$tab_array = array();
$tab_array[] = array(gettext("Client"), false, "pkg.php?xml=zerotier.xml");
$tab_array[] = array(gettext("Client Status"), true, "zerotier_status.php");
$tab_array[] = array(gettext("Controller"), false, "pkg.php?xml=zerotiercontroller.xml");
$tab_array[] = array(gettext("Network Members"), false, "zerotier_controller_auth_member.php");

display_top_tabs($tab_array);
?>
<div class="panel panel-default">
	<div class="panel-heading"><h2 class="panel-title">ZeroTier Status</h2></div>
	<div class="panel-body">
	<div class="table-responsive">
	<table class="table table-striped table-hover table-condensed">
		<thead>
			<tr>
				<th>Network ID</th>
				<th>Name</th>
				<th>MAC<h>
				<th>Status</th>
				<th>Type</th>
				<th>Device</th>
				<th>Assigned IP</th>
			</tr>
		</thead>
		<tbody>	<?php
			$list = zerotier_list_joined_networks();
			array_shift($list);
			foreach ($list as $nets):
			?>
			<tr>
				<?php
				$nets = str_replace('200 listnetworks ', '', $nets);
				foreach (explode(' ', $nets) as $net):
				?>
					<td><?= htmlspecialchars($net) ?></td>
				<? endforeach ?>
			</tr>
			<? endforeach ?></tbody>
	</table>
	</div>
</div>
<?php include("foot.inc"); ?>