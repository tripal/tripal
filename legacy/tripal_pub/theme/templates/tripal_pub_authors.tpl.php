<?php
$pub = $node->pub;

// expand the pub to include the pubauthors.
$options = [
  'return_array' => 1,
  'order_by' => ['rank' => 'ASC'],
];
$pub = chado_expand_var($pub, 'table', 'pubauthor', $options);

// see if we have authors as contacts if so then we'll add this resource
$authors = $pub->pubauthor;
$has_contacts = FALSE;
if (count($authors) > 0) {
  foreach ($authors as $author) {
    // expand the author to include the pubauthor_contact table records
    $options = [
      'return_array' => 1,
      'include_fk' => [
        'contact_id' => [
          'type_id' => 1,
        ],
      ],
    ];
    $author = chado_expand_var($author, 'table', 'pubauthor_contact', $options);
    if ($author->pubauthor_contact) {
      $has_contacts = TRUE;
    }
  }
}

if ($has_contacts) { ?>
    <div class="tripal_pub-data-block-desc tripal-data-block-desc">Additional
        information about authors:
    </div> <?php
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['', 'Details'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  $rank = 1;
  foreach ($authors as $author) {

    // expand the author to include the contact information linked via the pubauthor_contact table
    $contact = $author->pubauthor_contact[0]->contact_id;
    $options = [
      'return_array' => 1,
      'include_fk' => [
        'type_id' => 1,
      ],
    ];
    $contact = chado_expand_var($contact, 'table', 'contactprop', $options);
    $properties = $contact->contactprop;
    $options = ['order_by' => ['rank' => 'ASC']];
    $properties = chado_expand_var($properties, 'field', 'contactprop.value', $options);

    // link the contact to it's node if one exists
    $contact_name = $author->givennames . " " . $author->surname;
    if (property_exists($contact, 'nid')) {
      $contact_name = l($contact_name, 'node/' . $contact->nid);
    }

    // Get some additional details about this contact if they exists.
    $details = '';
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
      $rank,
      $contact_name . $details,
    ];
    $rank++;
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