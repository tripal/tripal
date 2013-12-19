<?php
$pub = $node->pub;

// expand the pub to include the properties.
$options = array(
  'return_array' => 1,
  'order_by' => array('rank' => 'ASC'),
);
$pub = tripal_core_expand_chado_vars($pub,'table', 'pubprop', $options);
$pubprops = $pub->pubprop;
$properties = array();
if (is_array($pubprops)) {
  foreach ($pubprops as $property) {
    // skip the following properties as those are already on other templates
    if ($property->type_id->name == 'Abstract' or
        $property->type_id->name == 'Citation' or
        $property->type_id->name == 'Publication Dbxref' or
        $property->type_id->name == 'Authors' or
        $property->type_id->name == 'Structured Abstract Part')  {
      continue;
    }
    $property = tripal_core_expand_chado_vars($property,'field','pubprop.value');
    $properties[] = $property;
  }
}
// we'll keep track of the keywords so we can lump them into a single row
$keywords = array(); ?>
  <div id="tripal_pub-properties-box" class="tripal_pub-info-box tripal-info-box">
    <div class="tripal_pub-info-box-title tripal-info-box-title">More Details</div>
    <div class="tripal_pub-info-box-desc tripal-info-box-desc">Additional details for this publication include:</div>
    <table class="tripal_pub-table tripal-table tripal-table-horz">
      <tr>
        <th>Property Name</th>
        <th>Value</th>
      </tr> <?php
      if (count($properties) > 0) {
        $i = 0;
        foreach ($properties as $property) {
          if ($property->type_id->name == 'Keywords') {
            $keywords[] = $property->value;
            continue;
          }
          $class = 'tripal_pub-table-odd-row tripal-table-odd-row';
          if ($i % 2 == 0 ) {
             $class = 'tripal_pub-table-odd-row tripal-table-even-row';
          }
          $i++; 
          ?>
          <tr class="<?php print $class ?>">
            <td nowrap><?php print $property->type_id->name ?></td>
            <td><?php print $property->value ?></td>
          </tr><?php 
        } 
        if (count($keywords) > 0) {
          $class = 'tripal_pub-table-odd-row tripal-table-odd-row';
          if ($i % 2 == 0 ) {
             $class = 'tripal_pub-table-odd-row tripal-table-even-row';
          }
          $i++; 
          ?>
          <tr class="<?php print $class ?>">
            <td nowrap>Keywords</td>
            <td><?php print implode(', ', $keywords) ?></td>
          </tr><?php   
        }
        $class = 'tripal_pub-table-odd-row tripal-table-odd-row';
        if ($i % 2 == 0 ) {
           $class = 'tripal_pub-table-odd-row tripal-table-even-row';
        }
      } ?>
    <tr class="<?php print $class ?>">
      <td>Internal ID</td>
      <td style="text-align:justify;"><?php print $pub->pub_id; ?></td>
    </tr>
  </table> 
</div> <?php

