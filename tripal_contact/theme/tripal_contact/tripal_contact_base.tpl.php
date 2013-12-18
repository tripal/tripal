<?php
$node = $variables['node'];
$contact = $variables['node']->contact;

?>
<div id="tripal_contact-base-box" class="tripal_contact-info-box tripal-info-box">
  <div class="tripal_contact-info-box-title tripal-info-box-title">Details</div>
  <div class="tripal_contact-info-box-desc tripal-info-box-desc"></div>   

  <table id="tripal_contact-table-base" class="tripal_contact-table tripal-table tripal-table-vert">
    <tr class="tripal_contact-table-even-row tripal-table-even-row">
      <th>Name</th>
      <td><?php print $contact->name; ?></td>
    </tr>
    <tr class="tripal_contact-table-odd-row tripal-table-odd-row">
      <th>Contact Type</th>
      <td><?php print $contact->type_id->name; ?></td>
    </tr>
    <tr class="tripal_contact-table-even-row tripal-table-even-row">
      <th>Description</th>
      <td><?php print $contact->description; ?></td>
    </tr>
  </table> 
</div>
