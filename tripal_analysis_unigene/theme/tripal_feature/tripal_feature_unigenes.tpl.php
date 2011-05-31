<?php
$node = $variables['node'];
$feature = $node->feature;
$unigenes = $feature->tripal_analysis_unigene->unigenes;

// if this feature has a unigene then we want to show the box
if($unigenes){
  //dpm($unigenes);
?>
<div id="tripal_feature-unigenes-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Unigenes</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">This <?php print $feature->type_id->name ?> is part of the following unigenes:</div>
  <?php if(count($unigenes) > 0){ ?>
  <table id="tripal_feature-unigenes-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Unigene Name</th>
      <th>Analysis Name</th>
      <th>Sequence type in Unigene</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($unigenes as $unigene){
      $class = 'tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal-table-even-row';
      }
      ?>
      <tr class="<?php print $class ?>">
        <td><?php 
           if($unigene->nid){
              print "<a href=\"".url("node/$unigene->nid")."\">$unigene->unigene_name</a>";
           } else {
              print $unigene->unigene_name;
           }?>
        </td>
        <td><?php 
           if($analysis->nid){
              print "<a href=\"".url("node/$analysis->nid")."\">$analysis->name</a>";
           } else {
              print $analysis->name;
           }?>
        </td>
        <td nowrap><?php 
           if($unigene->singlet){
              print "Singlet";
           } else {
              print $feature->type_id->name;
           }?>
        </td>
      </tr>
      <?php
      $i++;  
    } ?>
  </table>
  <?php } else { ?>
    <div class="tripal-no-results">There are no unigenes for this feature</div> 
  <?php }?>
</div>
<?php 
}
?>
