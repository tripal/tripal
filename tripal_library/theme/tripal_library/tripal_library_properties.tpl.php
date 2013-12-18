<?php
$library = $node->library;

// expand the library to include the properties.
$library = tripal_core_expand_chado_vars($library, 'table', 'libraryprop', array('return_array' => 1));
$libraryprops = $library->libraryprop;
$properties = array();

if (count($libraryprops) > 0) {
  foreach ($libraryprops as $property) {
    // we want to keep all properties but the library_description as that
    // property is shown on the base template page.
    if($property->type_id->name != 'library_description') {
      $property = tripal_core_expand_chado_vars($property,'field','libraryprop.value');
      $properties[] = $property;
    }
  }
}

if (count($properties) > 0) { ?>
  <div id="tripal_library-properties-box" class="tripal_library-info-box tripal-info-box">
    <div class="tripal_library-info-box-title tripal-info-box-title">Properties</div>
    <div class="tripal_library-info-box-desc tripal-info-box-desc">Properties for this library include:</div>
    <table class="tripal_library-table tripal-table tripal-table-horz">
      <tr>
        <th>Property Name</th>
        <th>Value</th>
      </tr> <?php
      $i = 0;
      foreach ($properties as $property) {
        $class = 'tripal_library-table-odd-row tripal-table-odd-row';
        if ($i % 2 == 0 ) {
           $class = 'tripal_library-table-odd-row tripal-table-even-row';
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
