<?php
$featuremap  = $variables['node']->featuremap;

// expand featuremap to include pubs 
$featuremap = tripal_core_expand_chado_vars($featuremap, 'table', 'featuremap_pub');
$pubs = $featuremap->featuremap_pub;
$pubs = tripal_core_expand_chado_vars($pubs, 'field', 'pub.title', array('return_array' => 1));
?>

<div id="tripal_featuremap-pub-box" class="tripal_featuremap-info-box tripal-info-box">
  <div class="tripal_featuremap-info-box-title tripal-info-box-title">Publications</div>
  <div class="tripal_featuremap-info-box-desc tripal-info-box-desc"></div>

  <table id="tripal_featuremap-pub-table" class="tripal_featuremap-table tripal-table tripal-table-vert" style="border-bottom:solid 2px #999999">
    <tr>
      <th>Year</th>
      <th>Reference</th>
      <th>Title</th></tr> <?php
      $i = 0;
      foreach ($pubs AS $pub) {
        $class = 'tripal_featuremap-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_featuremap-table-odd-row tripal-table-even-row';
        } ?>
    	  <tr class="<?php print $class ?>">
    	    <td><?php print $pub->pub_id->pyear ?></td>
    	    <td><?php print $pub->pub_id->uniquename ?></td>
    	    <td><?php print $pub->pub_id->title ?></td>
    	  </tr><?php 
    	  $i++;
      }  ?>
  </table>
</div>
