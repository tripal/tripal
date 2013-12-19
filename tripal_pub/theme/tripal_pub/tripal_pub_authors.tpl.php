<?php
$pub = $node->pub;

// expand the pub to include the pubauthors.
$options = array(
  'return_array' => 1,
  'order_by' => array('rank' => 'ASC'),
);
$pub = tripal_core_expand_chado_vars($pub, 'table', 'pubauthor', $options);

// see if we have authors as contacts if so then we'll add this resource
$authors = $pub->pubauthor;
$has_contacts = FALSE;
if (count($authors) > 0) { 
  foreach ($authors as $author) {
    // expand the author to include the pubauthor_contact table records
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'contact_id' => array(
          'type_id' => 1,
        ),
      ),
    );
    $author = tripal_core_expand_chado_vars($author, 'table', 'pubauthor_contact', $options);
    if ($author->pubauthor_contact) {
      $has_contacts = TRUE;
    }
  }
}

if ($has_contacts) { ?>
  <div id="tripal_pub-pubauthors-box" class="tripal_pub-info-box tripal-info-box">
    <div class="tripal_pub-info-box-title tripal-info-box-title">Authors</div>
    <div class="tripal_pub-info-box-desc tripal-info-box-desc">Additional information about authors:</div>
    <table id="tripal_pubauthor_<?php print $rank?>-table" class="tripal_pub-table tripal-table tripal-table-horz"><?php 
      $rank = 1;
      foreach ($authors as $author) {
         
        // expand the author to include the contact information linked via the pubauthor_contact table
        $contact = $author->pubauthor_contact[0]->contact_id;
        $options = array(
          'return_array' => 1,
          'include_fk' => array(
            'type_id' => 1,       
          ),      
        );
        $contact = tripal_core_expand_chado_vars($contact, 'table', 'contactprop', $options);
        $properties = $contact->contactprop;
        $options = array('order_by' => array('rank' => 'ASC'));
        $properties = tripal_core_expand_chado_vars($properties, 'field', 'contactprop.value', $options); 
        
        $class = 'tripal_pub-table-odd-row tripal-table-odd-row';
        if($rank % 2 == 0 ){
           $class = 'tripal_pub-table-even-row tripal-table-even-row';
        } ?>            
        <tr class="<?php print $class?>">
          <td><?php print $rank?></td>
          <?php
          // now build the table for display the authors and their information 
          if ($contact->nid) {?>
            <td><?php print l($author->givennames . " " . $author->surname, 'node/' . $contact->nid) ?></td><?php
          }
          else {?>
            <td><?php print $author->givennames . " " . $author->surname ?></td><?php
          } ?>
          <td> 
            <table class="tripal-subtable"><?php 
              if (is_array($properties)) {          
                foreach ($properties as $property) {
                  // skip the description and name properties
                  if ($property->type_id->name == "contact_description" or
                      $property->type_id->name == "Surname" or
                      $property->type_id->name == "Given Name" or
                      $property->type_id->name == "First Initials" or
                      $property->type_id->name == "Suffix") {
                    continue;
                  }?>
                  <tr>
                    <td><?php print $property->type_id->name ?></td>
                    <td>:</td>
                    <td><?php print $property->value ?></td>
                  </tr><?php
                  $i++; 
                } 
              }?> 
            </table>
          </td>
        </tr><?php 
        $rank++;
      }?>
    </table>
  </div><?php      
}