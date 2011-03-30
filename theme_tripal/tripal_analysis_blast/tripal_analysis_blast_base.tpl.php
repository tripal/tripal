<?php
$node = $variables['node'];
$analysis = $variables['node']->analysis;
$blast = $node->analysis->tripal_analysis_blast;

?>
<div id="tripal_analysis_blast-base-box" class="tripal_analysis_blast-info-box tripal-info-box">
  <div class="tripal_analysis_blast-info-box-title tripal-info-box-title">Blast Analysis Details</div>
  <div class="tripal_analysis_blast-info-box-desc tripal-info-box-desc"></div>
   <table id="tripal_analysis_blast-table-base" class="tripal_analysis_blast-table tripal-table tripal-table-vert">
      <tr class="tripal_analysis_blast-table-odd-row tripal-table-even-row">
        <th>Analysis Name</th>
        <td><?php print $analysis->name; ?></td>
      </tr>
      <tr class="tripal_analysis_blast-table-odd-row tripal-table-odd-row">
        <th nowrap>Software</th>
        <td><?php 
          print $analysis->program; 
          if($analysis->programversion){
             print " (" . $analysis->programversion . ")"; 
          }
          if($analysis->algorithm){
             print ". " . $analysis->algorithm; 
          }
          ?>
        </td>
      </tr>
      <tr class="tripal_analysis_blast-table-odd-row tripal-table-even-row">
        <th nowrap>Source</th>
        <td><?php 
          if($analysis->sourceuri){
             print "<a href=\"$analysis->sourceuri\">$analysis->sourcename</a>"; 
          } else {
             print $analysis->sourcename; 
          }
          if($analysis->sourceversion){
             print " (" . $analysis->sourceversion . ")"; 
          }
          ?>
          </td>
      </tr>
      <tr class="tripal_analysis_blast-table-odd-row tripal-table-odd-row">
        <th nowrap>Date performed</th>
        <td><?php print preg_replace("/^(\d+-\d+-\d+) .*/","$1",$analysis->timeexecuted); ?></td>
      </tr>
      <tr class="tripal_analysis_blast-table-odd-row tripal-table-even-row">
        <th nowrap>Description</th>
        <td><?php print $analysis->description; ?></td>
      </tr> 
      <tr class="tripal_analysis_blast-table-even-row tripal-table-odd-row">
        <th nowrap>Database</th>
        <td><?php print $blast->db->name ?></td>
      </tr> 
      <tr class="tripal_analysis_blast-table-even-row tripal-table-even-row">
        <th nowrap>Blast Arguments</th>
        <td><?php print $blast->blastparameters ?></td>
      </tr>  
      <tr class="tripal_analysis_blast-table-odd-row tripal-table-odd-row">
        <th>Report</th>
      </tr>       	                                
   </table>   
</div>
