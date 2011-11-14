<?php
$node = $variables['node'];
$organism = $node->organism;
$unigenes = $organism->tripal_analysis_unigene->unigenes;

//dpm($unigenes);
?>
<div id="tripal_organism-unigenes-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Unigenes</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">Below is a list of unigenes available for <i><?php print $organism->genus ?> <?php print $organism->species ?></i>. Click the unigene name for further details.</div>
  <?php if(count($unigenes) > 0){ ?>
  <table id="tripal_organism-unigenes-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Unigene Name</th>
      <th>Analysis Name</th>
      <th>Date Constructed</th>
      <th>Stats</th>
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
        <td>
           <?php 
           if($unigene->nid){
              print "<a href=\"".url("node/$unigene->nid")."\">$unigene->unigene_name</a>";
           } else {
              print $unigene->unigene_name;
           }?>
        </td>
        <td><?php print $unigene->name; ?></td>
        <td><?php print preg_replace("/^(\d+-\d+-\d+) .*/","$1",$unigene->timeexecuted); ?></td>
        <td nowrap>
             <?php if($unigene->num_reads){print "Reads: $unigene->num_reads<br>";} ?>
             <?php if($unigene->num_clusters){print "Clusters: $unigene->num_clusters<br>";} ?>
             <?php if($unigene->num_contigs){print "Contigs: $unigene->num_contigs<br>";} ?>
             <?php if($unigene->num_singlets){print "Singlets: $unigene->num_singlets<br>";} ?>
        </td>
      </tr>
      <?php
      $i++;  
    } ?>
  </table>
  <?php } else { ?>
    <div class="tripal-no-results">There are no unigenes for this organism</div> 
  <?php }?>
</div>
