<?php
$phylotree = $variables['node']->phylotree;

if ($phylotree->has_features) { ?>
    <div id="phylotree-organisms">
        <!-- d3 will add svg to this div -->
    </div> <?php
}
