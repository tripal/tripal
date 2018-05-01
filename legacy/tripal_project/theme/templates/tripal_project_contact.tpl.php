<?php
$project = $variables['node']->project;

// expand the project object to include the contacts from the project_contact
// table in chado.
$project = chado_expand_var($project, 'table', 'project_contact', ['return_array' => 1]);
$project_contacts = $project->project_contact;

if (count($project_contacts) > 0) { ?>
    <div class="tripal_project-data-block-desc tripal-data-block-desc">The
        following indivuals or groups have particpated in development or
        execution of this project
    </div><?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['', 'Details'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];
  $i = 1;
  foreach ($project_contacts as $project_contact) {
    $contact = $project_contact->contact_id;
    $contact_name = $contact->name;
    if (property_exists($contact, 'nid')) {
      $contact_name = l($contact_name, 'node/' . $contact->nid, ['attributes' => ['target' => '_blank']]);
    }

    // Get some additional details about this contact if they exists.
    $details = '';
    $options = ['return_array' => 1];
    $contact = chado_expand_var($contact, 'table', 'contactprop', $options);
    $properties = $contact->contactprop;
    $options = ['order_by' => ['rank' => 'ASC']];
    $properties = chado_expand_var($properties, 'field', 'contactprop.value', $options);

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
        $details .= "<br>" . $property->type_id->name . " : " . $property->value;
      }
    }

    $rows[] = [
      $i,
      $contact_name . $details,
    ];
    $i++;
  }
  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal_pub-table-contacts',
      'class' => 'tripal-data-table',
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
}
