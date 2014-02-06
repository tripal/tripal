<?php
$project = $variables['node']->project;

// expand the project object to include the contacts from the project_contact
// table in chado.
$project = tripal_core_expand_chado_vars($project,'table','project_contact', array('return_array' => 1));
$project_contacts = $project->project_contact;

if (count($project_contacts) > 0) { ?>
  <div class="tripal_project-data-block-desc tripal-data-block-desc">The following indivuals or groups have particpated in development or execution of this project</div><?php     
  
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('', 'Details');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  $i = 1;
  foreach ($project_contacts as $project_contact) {
    $contact = $project_contact->contact_id;
    $contact_name = $contact->name;
    if (property_exists($contact, 'nid')) {
      $contact_name = l($contact_name, 'node/' . $contact->nid, array('attributes' => array('target' => '_blank')));
    }
    
    // Get some additional details about this contact if they exists.
    $details = '';
    $options = array('return_array' => 1);
    $contact = tripal_core_expand_chado_vars($contact, 'table', 'contactprop', $options);
    $properties = $contact->contactprop;
    $options = array('order_by' => array('rank' => 'ASC'));
    $properties = tripal_core_expand_chado_vars($properties, 'field', 'contactprop.value', $options);
    
    if (is_array($properties)) {
      foreach ($properties as $property) {
        // skip the description and name properties
        if ($property->type_id->name == "contact_description" or
            $property->type_id->name == "Surname" or
            $property->type_id->name == "Given Name" or
            $property->type_id->name == "First Initials" or
            $property->type_id->name == "Suffix") {
          continue;
        }
        $details .= "<br>" . $property->type_id->name . " : " .  $property->value;
      }
    }
    
    $rows[] = array(
      $i,
      $contact_name . $details,
    );
    $i++;
  } 
      // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'tripal_pub-table-contacts',
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  
  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
}
