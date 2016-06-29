<?php
$phylotree = $variables['node']->phylotree; 

if ($phylotree->has_nodes) { ?>
  <div id="phylotree-radial-graph">
    <!-- d3 will add svg to this div -->
  </div> <?php 
} ?>
