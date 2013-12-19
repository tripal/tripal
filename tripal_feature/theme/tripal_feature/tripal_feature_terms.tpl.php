<?php

$feature = $node->feature;
$feature = tripal_core_expand_chado_vars($feature, 'table', 'feature_cvterm');

$terms = $feature->feature_cvterm;
if (!$terms) {
  $terms = array();
} elseif (!is_array($terms)) { 
  $terms = array($terms); 
}

// order the terms by CV
$s_terms = array();
foreach ($terms as $term) {
  $s_terms[$term->cvterm_id->cv_id->name][] = $term;  
}

if (count($s_terms) > 0) { ?>
  <div id="tripal_feature-terms-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Annotated Terms</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">The following terms have been associated with this <?php print $node->feature->type_id->name ?>:</div>  <?php
    // iterate through each term
    foreach ($s_terms as $cv => $terms) {  ?>
      <p><?php print ucwords(preg_replace('/_/', ' ', $cv)) ?></p>
      <table class="tripal_feature-table tripal-table tripal-table-horz">
        <tr>
          <th>Term</th>
          <th>Definition</th>
        </tr> <?php
        $i = 0;
        foreach ($terms as $term) { 
          $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
          if($i % 2 == 0 ){
            $class = 'tripal_feature-table-even-row tripal-table-even-row';
          }
          $accession = $term->cvterm_id->dbxref_id->accession;
          if (is_numeric($term->cvterm_id->dbxref_id->accession)) {
            $accession = $term->cvterm_id->dbxref_id->db_id->name . ":" . $term->cvterm_id->dbxref_id->accession;
          }
          if ($term->cvterm_id->dbxref_id->db_id->urlprefix) {
            $accession =  "<a href=\"" . $term->cvterm_id->dbxref_id->db_id->urlprefix . "$accession\" target=\"_blank\">$accession</a>";
          } ?>
          <tr class="<?php print $class ?>">
            <td><?php print $accession ?></td>
            <td><?php print $term->cvterm_id->name ?></td>
          </tr> <?php
        } ?>
      </table> <?php
  } ?>
  </div> <?php
} ?>
