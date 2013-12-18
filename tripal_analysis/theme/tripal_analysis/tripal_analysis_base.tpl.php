<?php

$analysis = $variables['node']->analysis;

// the description is a text field so we want to expand that
$analysis = tripal_core_expand_chado_vars($analysis,'field','analysis.description');


?>
<div id="tripal_analysis-base-box" class="tripal_analysis-info-box tripal-info-box">
  <div class="tripal_analysis-info-box-title tripal-info-box-title">Details</div>
   <table id="tripal_analysis-table-base" class="tripal_analysis-table tripal-table tripal-table-vert">
      <tr class="tripal_analysis-table-odd-row tripal-table-even-row">
        <th>Analysis Name</th>
        <td><?php print $analysis->name; ?></td>
      </tr>
      <tr class="tripal_analysis-table-odd-row tripal-table-odd-row">
        <th nowrap>Software</th>
        <td><?php 
          print $analysis->program; 
          if($analysis->programversion and $analysis->programversion != 'n/a'){
             print " (" . $analysis->programversion . ")"; 
          }
          if($analysis->algorithm){
             print ". " . $analysis->algorithm; 
          }
          ?>
        </td>
      </tr>
      <tr class="tripal_analysis-table-odd-row tripal-table-even-row">
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
      <tr class="tripal_analysis-table-odd-row tripal-table-odd-row">
        <th nowrap>Date performed</th>
        <td><?php print preg_replace("/^(\d+-\d+-\d+) .*/","$1",$analysis->timeexecuted); ?></td>
      </tr>
      <tr class="tripal_analysis-table-odd-row tripal-table-even-row">
        <th nowrap>Materials & Methods</th>
        <td><?php print $analysis->description; ?></td>
      </tr>             	                                
   </table>   
</div>
