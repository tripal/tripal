<?php

namespace Drupal\tripal_chado\Form;

use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\Core\Link;
use Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable;

class ChadoMviewForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_mview_form';
  }


  /**
   * A Form to Create/Edit a Custom table.
   */
  public function buildForm(array $form, FormStateInterface $form_state, $mview_id = null) {

    $chado = \Drupal::service('tripal_chado.database');

    if (!$mview_id) {
      $action = 'Add';
    }
    else {
      $action = 'Edit';
    }

    // Set form defaults.
    $default_table_schema = '';
    $default_force_drop = 0;
    $default_comment = '';
    $default_sql_query = '';
    $default_chado_schema = $chado->getSchemaName();
    $mview_is_locked = 'FALSE';

    // If this is an edit then set the form defaults differently.
    if (strcmp($action, 'Edit') == 0) {
      $mviews = \Drupal::service('tripal_chado.materialized_views');
      $mview = $mviews->loadById($mview_id);
      
      // Get the locked status of this mview (from the parent table).
      $mview_is_locked = $mview->isLocked();

      // set the default values.  If there is a value set in the
      // form_state then let's use that, otherwise, we'll pull
      // the values from the database
      $default_table_schema = var_export($mview->getTableSchema(), 1);
      $default_table_schema = preg_replace('/=>\s+\n\s+array/', '=> array', $default_table_schema);
      if ($form_state->getValue('table_schema')) {
        $default_table_schema = $form_state->getValue('table_schema');
      }

      // Get the default comment value.
      $default_comment = $mview->comment();
      if ($form_state->getValue('comment')) {
        $default_comment = $form_state->getValue('comment');
      }

      // Get the default SQL Query value.
      $default_sql_query = $mview->getSqlQuery();
      if ($form_state->getValue('sql_query')) {
        $default_sql_query = $form_state->getValue('sql_query');
      }

      // Get the default force drop value.
      if ($form_state->getValue('force_drop')) {
        $default_force_drop = $form_state->getValue('force_drop');
      }

      // Get the default Chado value.
      $default_chado_schema = $mview->getChadoSchema();
      if ($form_state->getValue('chado_schema')) {
        $default_chado_schema = $form_state->getValue('chado_schema');
      }
    }

    // Emit a warning if this table is locked, explaining why the submit button
    // is disabled.
    if ($action == 'Edit' && $mview_is_locked) {
      $messenger = \Drupal::service('messenger');
      $messenger->addWarning('This materialized view is locked and therefore cannot be edited.');
    }

    // Build the form
    $form['action'] = [
      '#type' => 'value',
      '#value' => $action,
    ];

    $form['mview_id'] = [
      '#type' => 'value',
      '#value' => $mview_id,
    ];

    $form['instructions'] = [
      '#type' => 'details',
      '#title' => 'Instructions',
      '#open' => False,
    ];

    $form['instructions']['text'] = [
      '#type' => 'item',
      '#markup' => '<p>' . t('Materialized views are used to help speed data
      querying, particularly for searching.  A materialized view is essentially
      a database table that is pre-populated with the desired data to search on.
      Rows in the materialized view are typically a combination of data from
      multiple tables with indexes on searchable columns. The table structure
      for materialized views is defined using the ' .
      Link::fromTextAndUrl('Drupal Schema API', Url::fromUri('https://api.drupal.org/api/drupal/includes!database!schema.inc/group/schemaapi/7',
          ['attributes' => ['target' => '_blank']]))->toString() . '</p>' .
      'Additionally, an SQL statement should be provided that populates the table with data. ' .
      '<p>Please note that table names should be all lower-case.</p>'),
    ];


    $form['instructions']['example'] = [
      '#type' => 'item',
      '#markup' => "Example Schema API definition for a materialized view: <pre>
[
  'description' => 'Stores the type and number of features per organism',
  'table' => 'organism_feature_count',
  'fields' => [
    'organism_id' => [
      'type' => 'int',
      'not null' => true,
    ],
    'genus' => [
      'type' => 'varchar',
      'length' => '255',
      'not null' => true,
    ],
    'species' => [
      'type' => 'varchar',
      'length' => '255',
      'not null' => true,
    ],
    'common_name' => [
      'type' => 'varchar',
      'length' => '255',
      'not null' => false,
    ],
    'num_features' => [
      'type' => 'int',
      'not null' => true,
    ],
    'cvterm_id' => [
      'type' => 'int',
      'not null' => true,
    ],
    'feature_type' => [
      'type' => 'varchar',
      'length' => '255',
      'not null' => true,
    ],
  ],
  'indexes' => [
    'organism_id_idx'  => ['organism_id'],
    'cvterm_id_idx'    => ['cvterm_id'],
    'feature_type_idx' => ['feature_type'],
  ],
]
</pre>",
    ];

    $form['instructions']['example_sql'] = [
      '#type' => 'item',
      '#markup' => "An example SQL statement to populate the table: <pre>
SELECT
    O.organism_id, O.genus, O.species, O.common_name,
    count(F.feature_id) as num_features,
    CVT.cvterm_id, CVT.name as feature_type
 FROM organism O
    INNER JOIN feature F ON O.Organism_id = F.organism_id
    INNER JOIN cvterm CVT ON F.type_id = CVT.cvterm_id
 GROUP BY
    O.Organism_id, O.genus, O.species, O.common_name, CVT.cvterm_id, CVT.name
</pre>",
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

    $form['sql_query'] = [
      '#type' => 'textarea',
      '#title' => t('SQL Query'),
      '#description' => t('Please enter the SQL statement used to populate the table.'),
      '#required' => TRUE,
      '#default_value' => $default_sql_query,
      '#rows' => 25,
      '#attributes' => [
        'style' => "font-family:Consolas,Monaco,Lucida Console,Liberation Mono,DejaVu Sans Mono,Bitstream Vera Sans Mono,Courier New, monospace;",
      ],
    ];

    $form['comment'] = [
      '#type' => 'textarea',
      '#title' => t('Description'),
      '#description' => t('Please provide a description of the purpose for this materialized view.'),
      '#required' => FALSE,
      '#default_value' => $default_comment,
    ];

    if ($action == 'Add') {
      $form['locked'] = [
        '#type' => 'select',
        '#title' => t('Locked'),
        '#description' => t('Decide whether this materialized view should be locked or not. This will also set the custom table to be locked as well'),
        '#options' => [
          False => t('No'),
          True => t('Yes'),
        ],
        '#default_value' => False, // Set the default value to not locked
      ];
    }


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
      '#disabled' => $mview_is_locked == 'TRUE' ? TRUE : FALSE,
    ];

    $form['cancel'] = [
      '#markup' => Link::fromTextAndUrl('Cancel', Url::fromUserInput('/admin/tripal/storage/chado/mviews'))->toString(),
    ];


    return $form;
  }

  /**
   * Validate the Create/Edit custom table form.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $action = $values['action'];
    $mview_id = $values['mview_id'];
    $chado_schema = $values['chado_schema'];
    $table_schema = $values['table_schema'];

    // Validate the contents of the table schema array.
    try {
      $schema_arr = [];
      eval("\$schema_arr = $table_schema;");
    }
    catch (ParseError $e) {
      $form_state->setErrorByName('schema', 'A parse error occured. Please check the syntax of your Schema Array to ensure it is a valid PHP array.');
    }
    $errors = ChadoCustomTable::validateTableSchema($schema_arr);
    foreach ($errors as $error) {
      $form_state->setErrorByName('schema', $error);
    }

    // When adding a new materialized view, make sure that it is not an existing materialized view. We check for uniqueness by combining table name and schema, as some sites may have more than one instance of Chado installed. 
    if ($action == 'Add') {
      $table = $schema_arr['table'];

      $mviews = \Drupal::service('tripal_chado.materialized_views');
      if ($mviews->loadByName($table, $chado_schema)) {
        $form_state->setErrorByName('schema', 'A materialized view based on that table in this schema already exists. Please choose a different table or schema, or edit the existing one.');
      }
    }
  }

  /**
   * Submit the Create/Edit Custom table form.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $action = $values['action'];
    $mview_id = $values['mview_id'];
    $chado_schema = $values['chado_schema'];
    $table_schema = $values['table_schema'];
    $force_drop = $values['force_drop'];
    $comment = $values['comment'];
    $sql_query = $values['sql_query'];
    $locked = $values['locked'];

    $mviews = \Drupal::service('tripal_chado.materialized_views');

    // convert the schema into a PHP array
    $schema_arr = [];
    eval("\$schema_arr = $table_schema;");

    if (strcmp($action, 'Edit') == 0) {
      $mview = $mviews->loadById($mview_id);
      $mview->setComment($comment);
      $mview->setSqlQuery($sql_query);
      $success = $mview->setTableSchema($schema_arr, $force_drop);
      if ($success) {
        \Drupal::messenger()->addMessage(t("The materialized view was succesfully updated."), 'status');
      }
      else {
        $link = Link::fromTextAndUrl(t('recent logs'), Url::fromUserInput('/admin/reports/dblog'))->toString();
        \Drupal::messenger()->addError(t("Could not update the materialized view. Please see the @logs for further details.",
            ['@logs' => $link]), 'status');
      }
    }
    elseif (strcmp($action, 'Add') == 0) {
      $mview = $mviews->create($schema_arr['table'], $chado_schema);
      $mview->setComment($comment);
      $mview->setSqlQuery($sql_query);
      $mview->setLocked($locked);
      $success = $mview->setTableSchema($schema_arr);
      if ($success) {
        \Drupal::messenger()->addMessage(t("The materialized view has been added."), 'status');
      }
      else {
        \Drupal::messenger()->addError(t("The materialized view could not be created. Please see logs for further details."), 'status');
      }
    }
    else {
      drupal_set_message(t("No action performed."));
    }

    $response = new RedirectResponse(\Drupal\Core\Url::fromUserInput('/admin/tripal/storage/chado/mviews')->toString());
    $response->send();
  }

}




?>
