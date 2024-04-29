<?php

namespace Drupal\tripal_chado\Form;

use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\Core\Link;
use Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable;

class ChadoCustomTableForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_custom_table_form';
  }


  /**
   * A Form to Create/Edit a Custom table.
   */
  public function buildForm(array $form, FormStateInterface $form_state, $table_id = null) {

    $chado = \Drupal::service('tripal_chado.database');
    $custom_tables = \Drupal::service('tripal_chado.custom_tables');

    if (!$table_id) {
      $action = 'Add';
    }
    else {
      $action = 'Edit';
    }

    // Set form defaults.
    $default_table_schema = '';
    $default_force_drop = 0;
    $default_chado_schema = $chado->getSchemaName();
    $table_is_locked = 'FALSE';

    // If this is an edit then set the form defaults differently.
    if (strcmp($action, 'Edit') == 0) {

      $custom_table = $custom_tables->loadById($table_id);

      // Do not allow edits if the custom table is locked. This will be used to disable the submit button on this form and warn the user.
      $table_is_locked = $custom_table->isLocked();

      // Get the default table schema.
      $default_table_schema = var_export($custom_table->getTableSchema(), 1);
      $default_table_schema = preg_replace('/=>\s+\n\s+array/', '=> array', $default_table_schema);
      if ($form_state->getValue('table_schema')) {
        $default_table_schema = $form_state->getValue('table_schema');
      }

      // Get the default force drop value.
      if ($form_state->getValue('force_drop')) {
        $default_force_drop = $form_state->getValue('force_drop');
      }

      // Get the default Chado schema.
      $default_chado_schema = $custom_table->getChadoSchema();
      if ($form_state->getValue('chado_schema')) {
        $default_chado_schema = $form_state->getValue('chado_schema');
      }
    }

    // Emit a warning if this table is locked, explaining why the submit button
    // is disabled.
    if ($action == 'Edit' && $table_is_locked) {
      $messenger = \Drupal::service('messenger');
      $messenger->addWarning('The Tripal module that provides this table has requested it to be locked for the proper functioning of the module and therefore no changes can be made.');
    }

    // Build the form
    $form['action'] = [
      '#type' => 'value',
      '#value' => $action,
    ];

    $form['table_id'] = [
      '#type' => 'value',
      '#value' => $table_id,
    ];

    $form['instructions'] = [
      '#type' => 'details',
      '#title' => 'Instructions',
      '#open' => False,
    ];

    $form['instructions']['text'] = [
      '#type' => 'item',
      '#markup' => '<p>' . t('At times it is necessary to add a custom table
         to the Chado schema. These are not offically sanctioned tables but may
         be necessary for local data requirements. Avoid creating custom tables
         when possible as other GMOD tools may not recognize these tables nor
         the data in them.  Linker tables or property tables are often a good
         candidate for a custom table. For example a table to link stocks and
         libraries (e.g. library_stock). Try to model linker or propery tables
         after existing tables.  If the table already exists it will not be
         modified.  To force dropping and recreation of the table
         click the checkbox below.  Tables are defined using the ' .
         Link::fromTextAndUrl('Drupal Schema API', Url::fromUri('https://api.drupal.org/api/drupal/includes!database!schema.inc/group/schemaapi/7',
            ['attributes' => ['target' => '_blank']]))->toString() . '</p>' .
          '<p>Please note that table names should be all lower-case.</p>'
        ),
    ];


    $form['instructions']['example'] = [
      '#type' => 'item',
      '#markup' => "Example library_stock table: <pre>[
  'table' => 'library_stock',
  'fields' => [
    'library_stock_id' => [
      'type' => 'serial',
      'not null' => TRUE,
    ],
    'library_id' => [
      'type' => 'int',
      'not null' => TRUE,
    ],
    'stock_id' => [
      'type' => 'int',
      'not null' => TRUE,
    ]
  ],
  'primary key' => [
    'library_stock_id'
  ],
  'unique keys' => [
    'library_stock_c1' => [
      'library_id',
      'stock_id'
    ]
  ],
  'foreign keys' => [
    'library' => [
      'table' => 'library',
      'columns' => [
        'library_id' => 'library_id'
      ],
    ],
    'stock' => [
      'table' => 'stock',
      'columns' => [
        'stock_id' => 'stock_id'
      ]
    ]
  ]
]</pre>",
    ];

    if ($action == 'Add') {
      $form['force_drop'] = [
        '#type' => 'value',
        '#value' => $default_force_drop,
      ];
    }
    else {
      $form['force_drop'] = [
        '#type' => 'checkbox',
        '#title' => t('Re-create table'),
        '#description' => t('Check this box if your table already exists and you would like to drop it and recreate it.'),
        '#default_value' => $default_force_drop,
      ];
    }

    if ($action == 'Add') {
      $chado_schemas = [];
      foreach ($chado->getAvailableInstances() as $schema_name => $details) {
        $chado_schemas[$schema_name] = $schema_name;
      }
      $form['chado_schema'] = [
        '#type' => 'select',
        '#title' => 'Chado Schema Name',
        '#required' => TRUE,
        '#description' => 'Select one of the installed Chado schemas to prepare..',
        '#options' => $chado_schemas,
        '#default_value' => $default_chado_schema,
      ];
    }
    else {
      $form['chado_schema'] = [
        '#type' => 'value',
        '#value' => $default_chado_schema,
      ];
      $form['chado_schema_item'] = [
        '#type' => 'item',
        '#title' => 'Chado Schema Name',
        '#markup' => $default_chado_schema
      ];
    }

    $form['table_schema'] = [
      '#type' => 'textarea',
      '#title' => t('Schema Array'),
      '#description' => t('Please enter the ' . Link::fromTextAndUrl('Drupal Schema API', Url::fromUri('https://api.drupal.org/api/drupal/includes!database!schema.inc/group/schemaapi/7', ['attributes' => ['target' => '_blank']]))->toString() . ' compatible array that defines the table.'),
      '#required' => TRUE,
      '#default_value' => $default_table_schema,
      '#rows' => 25,
    ];

    if ($action == 'Edit') {
      $value = 'Save';
    }
    if ($action == 'Add') {
      $value = 'Add';
    }

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t($value),
      '#executes_submit_callback' => TRUE,
      '#disabled' => $table_is_locked == 'TRUE' ? TRUE : FALSE,
    ];

    $form['cancel'] = [
      '#markup' => Link::fromTextAndUrl('Cancel', Url::fromUserInput('/admin/tripal/storage/chado/custom_tables'))->toString(),
    ];


    return $form;
  }

  /**
   * Validate the Create/Edit custom table form.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $action = $values['action'];
    $table_id = $values['table_id'];
    $chado_schema = $values['chado_schema'];
    $table_schema = $values['table_schema'];

    // Validate the contents of the table schema array.
    try {
      $schema_arr = [];
      eval("\$schema_arr = $table_schema;");
    }
    catch (ParseError $e) {
      $form_state->setErrorByName('schema', 'The schema array is not a valid PHP array. Please check the syntax.');
    }
    $errors = ChadoCustomTable::validateTableSchema($schema_arr);
    foreach ($errors as $error) {
      $form_state->setErrorByName('schema', $error);
    }
  }

  /**
   * Submit the Create/Edit Custom table form.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $action = $values['action'];
    $table_id = $values['table_id'];
    $chado_schema = $values['chado_schema'];
    $table_schema = $values['table_schema'];
    $force_drop = $values['force_drop'];

    $custom_tables = \Drupal::service('tripal_chado.custom_tables');

    // convert the schema into a PHP array
    $schema_arr = [];
    eval("\$schema_arr = $table_schema;");

    if (strcmp($action, 'Edit') == 0) {
      $custom_table = $custom_tables->loadById($table_id);
      $success = $custom_table->setTableSchema($schema_arr, $force_drop);
      if ($success) {
        \Drupal::messenger()->addMessage(t("The custom table was succesfully updated."), 'status');
      }
      else {
        $link = Link::fromTextAndUrl(t('recent logs'), Url::fromUserInput('/admin/reports/dblog'))->toString();
        \Drupal::messenger()->addError(t("Could not update the custom table. Please see the @logs for further details.",
            ['@logs' => $link]), 'status');
      }
    }
    elseif (strcmp($action, 'Add') == 0) {
      $custom_table = $custom_tables->create($schema_arr['table'], $chado_schema);
      $success = $custom_table->setTableSchema($schema_arr);
      if ($success) {
        \Drupal::messenger()->addMessage(t("Custom table has been added."), 'status');
      }
      else {
        \Drupal::messenger()->addError(t("Custom table could not be created. Please see logs for further details."), 'status');
      }
    }
    else {
      drupal_set_message(t("No action performed."));
    }

    $response = new RedirectResponse(\Drupal\Core\Url::fromUserInput('/admin/tripal/storage/chado/custom_tables')->toString());
    $response->send();
  }

}




?>
