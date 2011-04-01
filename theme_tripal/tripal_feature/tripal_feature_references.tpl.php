<?php

/*

References variables
--------------------

$variables['references']:  an array of references indexed 0 .. n, where 'n' the
  number of references available for this feature.

These variables are avaliable for each reference in the array:
  
  uniquename
  feature_id
  accession
  dbdesc
  db_id
  db_name
  urlprefix
  dbxref_id

*/
$references = $variables['tripal_feature']['references'];
$feature = $variables['node']->feature;
?>
<div id="tripal_feature-references-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">References</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">The feature '<?php print $feature->featurename ?>' is also available at these locations</div>
  <?php if(count($references) > 0){ ?>
  <table id="tripal_feature-references-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Dababase</th>
      <th>Accession</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($references as $result){ 
      $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_feature-table-odd-row tripal-table-even-row';
      }
      ?>
      <tr class="<?php print $class ?>">
        <td><?php print $result->db_name?></td>
        <td><?php 
           if($result->urlprefix){ 
              ?><a href="<?php print $result->urlprefix.$result->accession?>" target="_blank"><?php print $result->accession?></a><?php 
           } else { 
             print $result->accession; 
           } 
           ?>
        </td>
      </tr>
      <?php
      $i++;  
    } ?>
  </table>
  <?php } else { ?>
    <div class="tripal-no-results">There are no references for this feature</div> 
  <?php }?>
</div>
