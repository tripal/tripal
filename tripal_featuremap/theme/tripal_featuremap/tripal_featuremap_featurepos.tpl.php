<?php
$featuremap = $variables['node']->featuremap;
$feature_positions = array();

// expand the featuremap object to include the records from the featurepos table
// specify the number of features to show by default and the unique pager ID
$num_results_per_page = 25; 
$featurepos_pager_id = 0;

// get the features aligned on this map
$options = array(  
  'return_array' => 1,
  'order_by' => array('map_feature_id' => 'ASC'),
  'pager' => array('limit' => $num_results_per_page, 'element' => $featurepos_pager_id),
  'include_fk' => array(
    'map_feature_id' => array(
      'type_id' => 1,
      'organism_id' => 1,
    ),
    'feature_id' => array(
      'type_id' => 1,
    ),
    'featuremap_id' => array(
       'unittype_id' => 1,
    ),
  ),
);

$featuremap = tripal_core_expand_chado_vars($featuremap, 'table', 'featurepos', $options);
$feature_positions = $featuremap->featurepos;

// create the pager.  
global $pager_total_items;
$featurepos_pager = theme('pager', array(), $num_results_per_page, $featurepos_pager_id, array('block' => 'featurepos'));
$total_features = $pager_total_items[$featurepos_pager_id];


if(count($feature_positions) > 0){ ?>
  <div id="tripal_featuremap-featurepos-box" class="tripal_featuremap-info-box tripal-info-box">
    <div class="tripal_featuremap-info-box-title tripal-info-box-title">Map Features</div>
    <div class="tripal_featuremap-info-box-desc tripal-info-box-desc">This Map contains <?php print number_format($total_features) ?> features:</div>
    <table id="tripal_featuremap-featurepos-table" class="tripal_featuremap-table tripal-table tripal-table-horz">
      <tr>
        <th>Landmark</th>
        <th>Type</th>
        <th>Organism</th>
        <th>Feature Name</th>
        <th>Type</th>
        <th>Position</th>
      </tr> <?php
      $i = 0; 
      foreach ($feature_positions as $position){
        $map_feature = $position->map_feature_id;
        $feature = $position->feature_id;  
        $organism = $map_feature->organism_id; 

        // check if there are any values in the featureposprop table for the start and stop
        $mappos = $position->mappos;
        $options = array(
          'return_array' => 1,
          'include_fk' => array(
            'type_id' => 1,            
          ),
        );
        $position = tripal_core_expand_chado_vars($position, 'table', 'featureposprop', $options);
        $featureposprop = $position->featureposprop;
        if (is_array($featureposprop)) {
          foreach ($featureposprop as $index => $property) {
             if ($property->type_id->name == 'start') {
               $start = $property->value;
             }
             if ($property->type_id->name == 'stop') {
               $stop = $property->value;
             }
          }      
        }  
        if ($start and $stop and $start != $stop) {
          $mappos = "$start-$stop";
        }
        if ($start and !$stop) {
          $mappos = $start;
        } 
        if ($start and $stop and $start == $stop) {
          $mappos = $start;
        }
        
        $class = 'tripal_featuremap-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_featuremap-table-even-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td> <?php 
            if ($map_feature->nid) { 
              print l($map_feature->name, 'node/' . $map_feature->nid, array('attributes' => array('target' => '_blank')));
            } 
            else { 
              print $map_feature->name;
            } ?>
          </td>          
          <td><?php print $map_feature->type_id->name ?></td>
          <td><?php 
            if ($organism->nid) { 
              print l($organism->genus . ' ' . $organism->species, 'node/' . $organism->nid, array('attributes' => array('target' => '_blank')));
            } 
            else { 
              print $organism->genus . ' ' . $organism->species;
            } ?>
          </td>
          <td> <?php 
            if ($feature->nid) { 
              print l($feature->name, 'node/' . $feature->nid, array('attributes' => array('target' => '_blank')));
            } 
            else { 
              print $feature->name;
            } ?>
          </td>
          <td><?php print $feature->type_id->name ?></td>
          <td><?php print $mappos ?>&nbsp;<?php print $position->featuremap_id->unittype_id->name ?> </td>
        </tr> <?php
        $i++;  
      } ?>
    </table> <?php 
    print $featurepos_pager ?>
  </div><?php 
}?>

