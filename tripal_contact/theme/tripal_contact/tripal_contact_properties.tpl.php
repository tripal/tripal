<?php
$contact = $node->contact;

// expand the contact to include the properties.
$contact = tripal_core_expand_chado_vars($contact,'table', 'contactprop', array('return_array' => 1));
$contactprops = $contact->contactprop;
$properties = array();
if (is_array($contactprops)) {
  foreach ($contactprops as $property) {
    // we want to keep all properties but the contact_description as that
    // property is shown on the base template page.
    if($property->type_id->name != 'contact_description') {
      $property = tripal_core_expand_chado_vars($property,'field','contactprop.value');
      $properties[] = $property;
    }
  }
}

if (count($properties) > 0) { ?>
  <div id="tripal_contact-properties-box" class="tripal_contact-info-box tripal-info-box">
    <div class="tripal_contact-info-box-title tripal-info-box-title">More Details</div>
    <div class="tripal_contact-info-box-desc tripal-info-box-desc">Additional information about this contact:</div>
    <table class="tripal_contact-table tripal-table tripal-table-horz">
      <tr>
        <th>Property Name</th>
        <th>Value</th>
      </tr> <?php
      $i = 0;
      foreach ($properties as $property) {
        $class = 'tripal_contact-table-odd-row tripal-table-odd-row';
        if ($i % 2 == 0 ) {
           $class = 'tripal_contact-table-odd-row tripal-table-even-row';
        }
        $i++; 
        ?>
        <tr class="<?php print $class ?>">
          <td><?php print ucfirst(preg_replace('/_/', ' ', $property->type_id->name)) ?></td>
          <td><?php print $property->value ?></td>
        </tr><?php 
      } ?>
    </table>
  </div> <?php
}
