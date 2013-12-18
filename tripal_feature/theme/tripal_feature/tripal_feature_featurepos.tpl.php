<?php
$feature = $variables['node']->feature;
$map_positions = array();

// expand the feature object to include the records from the featurepos table
// specify the number of features to show by default and the unique pager ID
$num_results_per_page = 25; 
$featurepos_pager_id = 0;

// get the maps associated with this feature
$options = array(  
  'return_array' => 1,
  'order_by' => array('map_feature_id' => 'ASC'),
  'pager' => array('limit' => $num_results_per_page, 'element' => $featurepos_pager_id),
  'include_fk' => array(
    'map_feature_id' => array(
      'type_id' => 1,
      'organism_id' => 1,
    ),
    'featuremap_id' => array(
       'unittype_id' => 1,
    ),
  ),
);

$feature = tripal_core_expand_chado_vars($feature, 'table', 'featurepos', $options);

// because the featurepos table has  FK relationships with map_feature_id and feature_id with the feature table 
// the function call above will try to expand both and will create an array of matches for each FK.
// we only want to show the map that this feature belongs to
$map_positions = $feature->featurepos->feature_id;

// create the pager.  
$featurepos_pager = theme('pager', array(), $num_results_per_page, $featurepos_pager_id, array('block' => 'featurepos'));


if(count($map_positions) > 0){ ?>
  <div id="tripal_feature-featurepos-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Maps</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">This feature is contained in the following maps:</div>
    <table id="tripal_feature-featurepos-table" class="tripal_feature-table tripal-table tripal-table-horz">
      <tr>
        <th>Map Name</th>
        <th>Landmark</th>
        <th>Type</th>
        <th>Position</th>
      </tr> <?php
      $i = 0; 
      foreach ($map_positions as $position){
        $map_feature = $position->map_feature_id;

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
        
        $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_feature-table-even-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td> <?php 
            if ($position->featuremap_id->nid) { 
              print l($position->featuremap_id->name, 'node/' . $position->featuremap_id->nid, array('attributes' => array('target' => '_blank')));
            } 
            else { 
              print $position->featuremap_id->name;
            } ?>
          </td>
          <td> <?php 
            if ($map_feature->nid) { 
              print l($map_feature->name, 'node/' . $map_feature->nid, array('attributes' => array('target' => '_blank')));
            } 
            else { 
              print $map_feature->name;
            } ?>
          </td>          
          <td><?php print $map_feature->type_id->name ?></td>
          <td><?php print $mappos ?> <?php print $position->featuremap_id->unittype_id->name ?> </td>
        </tr> <?php
        $i++;  
      } ?>
    </table> <?php 
    print $featurepos_pager ?>
  </div><?php 
}?>

