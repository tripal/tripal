<?php

namespace Drupal\tripal_chado\Hook;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

use Drupal\views\Plugin\views\query\QueryPluginBase;

/**
 * Hook implementations for the Tripal Chado module.
 */
class TripalChadoHooks {

  use StringTranslationTrait;

  /**
   * Implements hook_help().
   */
  #[Hook('help')]
  public function help($route_name, RouteMatchInterface $route_match): string|array|null {
    switch ($route_name) {
      // Main module help for the tripal_chado module.
      case 'help.page.tripal_chado':
        $output = '<h3>' . $this->t('About') . '</h3>';
        $output .= '<p>' . $this->t('Chado integration for Tripal.') . '</p>';
        return ['#markup' => $output];
    }
    return NULL;
  }

  /**
   * Implements hook_rebuild().
   */
  #[Hook('rebuild')]
  public function rebuild(): string {
    \Drupal::service('tripal_chado.rebuild_service')->executeRebuild();
    // Return value of the module name is only used for phpunit tests.
    return 'tripal_chado';
  }

  /**
   * Implements hook_form_alter().
   */
  #[Hook('form_alter')]
  public function formAlter(array &$form, FormStateInterface $form_state, $form_id) {

    // If this is the field_config_edit_form and if we are adding a Chado
    // field, then we need to add a submit hook to set the base table in the
    // content type's 3rd party settings. The field settings form doesn't allow
    // us to set a submit callback, so we have to add it using hook_form_alter.
    if ($form_id == 'field_config_edit_form') {
      // If we have 'storge_plugin_settings' specifying a 'base_table', then
      // this is a Chado field and we should add the callback on submit so
      // that we can set the 3rd party settings on the content type.
      // Retrieve base_table from the form state, if present.
      $base_table = $form_state->getValue([
        'field_storage',
        'subform',
        'settings',
        'storage_plugin_settings',
        'base_table',
      ]);
      if ($base_table) {
        // Add submit callback.
        $form['actions']['submit']['#submit'][] = [
          'Drupal\tripal_chado\TripalField\ChadoFieldItemBase',
          'storageSettingsFormSubmitBaseTable',
        ];
      }
    }
  }

  /**
   * Implements hook_config_schema_info_alter().
   *
   * Specifically, we are altering config schema set in the tripal module.
   * We use this approach to ensure we are extending the existing schema
   * which makes these changes available to extension modules defining their
   * own yml files.
   */
  #[Hook('config_schema_info_alter')]
  public function configSchemaInfoAlter(&$definitions) {

    // Third party settings for entity types
    // (always add to both collection + content type entity).
    // -- Collection.
    $ttetc_mapping = $definitions['tripal.tripalentitytype_collection.*']['mapping']['content_types']['sequence']['mapping']['settings']['mapping']
      ?? [];
    $ttetc_mapping['chado_base_table'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Base Chado Table',
      'nullable' => TRUE,
    ];
    $ttetc_mapping['bundle_type_table'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Table Specifying Content CV Term',
      'nullable' => TRUE,
    ];
    $ttetc_mapping['bundle_type_column'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Column Specifying Content CV Term',
      'nullable' => TRUE,
    ];
    $definitions['tripal.tripalentitytype_collection.*']['mapping']['content_types']['sequence']['mapping']['settings']['mapping']
      = $ttetc_mapping;

    // -- Content Type Entity.
    $tct_mapping = $definitions['tripal.content_type.*']['mapping']['third_party_settings']['mapping']['tripal']['mapping']
      ?? [];
    $tct_mapping['chado_base_table'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Base Chado Table',
      'nullable' => TRUE,
    ];
    $tct_mapping['bundle_type_table'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Table Specifying Content CV Term',
      'nullable' => TRUE,
    ];
    $tct_mapping['bundle_type_column'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Column Specifying Content CV Term',
      'nullable' => TRUE,
    ];
    $definitions['tripal.content_type.*']['mapping']['third_party_settings']['mapping']['tripal']['mapping']
      = $tct_mapping;

  }

  /**
   * Implements hook_views_data().
   */
  #[Hook('views_data')]
  public function viewsData() {
    $data = [];
    $this->viewsDataCustomTables($data);
    $this->viewsDataTripalMviews($data);
    $this->viewsDataTermTables($data);
    return $data;
  }

  /**
   * Describes the tripal_custom_tables table for Drupal views.
   *
   * @param array &$data
   *   Description to pass to hook_views_data.
   */
  protected function viewsDataCustomTables(&$data) {
    $data['tripal_custom_tables'] = [];
    $data['tripal_custom_tables']['table'] = [];
    $data['tripal_custom_tables']['table']['group'] = $this->t('Chado Custom Tables');
    $data['tripal_custom_tables']['table']['provider'] = 'tripal';
    $data['tripal_custom_tables']['table']['base'] = [
      'field' => 'table_id',
      'title' => $this->t('Tripal Custom Tables'),
      'help' => $this->t('Provides information about custom tables in Chado.'),
      'weight' => 10,
    ];
    $data['tripal_custom_tables']['table']['join'] = [
      'tripal_mviews' => [
        'left_field' => 'table_id',
        'field' => 'table_id',
      ],
    ];
    $data['tripal_custom_tables']['table']['join']['tripal_mviews'] = [
      'left_table' => 'tripal_mviews',
      'left_field' => 'table_id',
      'field' => 'table_id',
    ];

    // Table ID.
    $data['tripal_custom_tables']['table_id'] = [
      'title' => $this->t('Table ID'),
      'help' => $this->t('The custom table primary key.'),
      'field' => [
        'id' => 'numeric',
      ],
      'filter' => [
        'id' => 'numeric',
      ],
      'sort' => [
        'id' => 'standard',
      ],
      'argument' => [
        'id' => 'numeric',
      ],
      'relationship' => [
        'id' => 'standard',
        'base' => 'tripal_mviews',
        'base field' => 'table_id',
        'label' => $this->t('Materialized Views.'),
      ],
    ];

    // Table Name.
    $data['tripal_custom_tables']['table_name'] = [
      'title' => $this->t('Table Name'),
      'help' => $this->t('The name of the table.'),
      'field' => [
        'id' => 'standard',
      ],
      'sort' => [
        'id' => 'standard',
      ],
      'filter' => [
        'id' => 'string',
      ],
      'argument' => [
        'id' => 'string',
      ],
    ];

    $data['tripal_custom_tables']['locked'] = [
      'title' => $this->t('Locked'),
      'help' => $this->t('Indicates if the table is locked from end-users.'),
      'field' => [
        'id' => 'boolean',
      ],
      'sort' => [
        'id' => 'standard',
      ],
      'filter' => [
        'id' => 'string',
      ],
      'argument' => [
        'id' => 'string',
      ],
    ];

    $data['tripal_custom_tables']['chado'] = [
      'title' => $this->t('Chado Schema'),
      'help' => $this->t('The Chado schema in which the table is present.'),
      'field' => [
        'id' => 'standard',
      ],
      'sort' => [
        'id' => 'standard',
      ],
      'filter' => [
        'id' => 'string',
      ],
      'argument' => [
        'id' => 'string',
      ],
    ];

    $data['tripal_custom_tables']['edit_link'] = [
      'title' => $this->t('Edit Table'),
      'help' => $this->t('Clickable link to edit a custom table'),
      'field' => [
        'id' => 'chado_custom_tables_edit_link',
      ],
    ];

    $data['tripal_custom_tables']['delete_link'] = [
      'title' => $this->t('Delete Table'),
      'help' => $this->t('Clickable link to delete a custom table'),
      'field' => [
        'id' => 'chado_custom_tables_delete_link',
      ],
    ];
  }

  /**
   * Describes the tripal_mviews table for Drupal views.
   *
   * @param array &$data
   *   Description to pass to hook_views_data.
   */
  protected function viewsDataTripalMviews(&$data) {
    $data['tripal_mviews'] = [];
    $data['tripal_mviews']['table'] = [];
    $data['tripal_mviews']['table']['group'] = $this->t('Chado Materialized Views');
    $data['tripal_mviews']['table']['provider'] = 'tripal';
    $data['tripal_mviews']['table']['base'] = [
      'field' => 'mview_id',
      'title' => $this->t('Tripal Materialized views'),
      'help' => $this->t('Provides information about materialized views in Chado.'),
      'weight' => 10,
    ];

    // Mview ID.
    $data['tripal_mviews']['mview_id'] = [
      'title' => $this->t('Materialized View ID'),
      'help' => $this->t('The materialized view primary key.'),
      'field' => [
        'id' => 'numeric',
      ],
      'filter' => [
        'id' => 'numeric',
      ],
      'sort' => [
        'id' => 'standard',
      ],
      'argument' => [
        'id' => 'numeric',
      ],
    ];

    // Table ID.
    $data['tripal_mviews']['table_id'] = [
      'title' => $this->t('Custom Table ID'),
      'help' => $this->t('The custom table foreign key.'),
      'field' => [
        'id' => 'numeric',
      ],
      'filter' => [
        'id' => 'numeric',
      ],
      'sort' => [
        'id' => 'standard',
      ],
      'argument' => [
        'id' => 'numeric',
      ],
    ];

    // Table Name.
    $data['tripal_mviews']['name'] = [
      'title' => $this->t('Table Name'),
      'help' => $this->t('The name of the materialized view table.'),
      'field' => [
        'id' => 'standard',
      ],
      'sort' => [
        'id' => 'standard',
      ],
      'filter' => [
        'id' => 'string',
      ],
      'argument' => [
        'id' => 'string',
      ],
    ];

    // SQL Query.
    $data['tripal_mviews']['query'] = [
      'title' => $this->t('SQL Query'),
      'help' => $this->t('The SQL query used to populate the view.'),
      'field' => [
        'id' => 'standard',
      ],
      'sort' => [
        'id' => 'standard',
      ],
      'filter' => [
        'id' => 'string',
      ],
      'argument' => [
        'id' => 'string',
      ],
    ];

    // Status.
    $data['tripal_mviews']['status'] = [
      'title' => $this->t('Status'),
      'help' => $this->t('The status of the most recent population of the view.'),
      'field' => [
        'id' => 'standard',
      ],
      'sort' => [
        'id' => 'standard',
      ],
      'filter' => [
        'id' => 'string',
      ],
      'argument' => [
        'id' => 'string',
      ],
    ];

    // Comment.
    $data['tripal_mviews']['comment'] = [
      'title' => $this->t('Description'),
      'help' => $this->t('A description of this view.'),
      'field' => [
        'id' => 'standard',
      ],
      'sort' => [
        'id' => 'standard',
      ],
      'filter' => [
        'id' => 'string',
      ],
      'argument' => [
        'id' => 'string',
      ],
    ];

    // Last update.
    $data['tripal_mviews']['last_update'] = [
      'title' => $this->t('Last Update'),
      'help' => $this->t('A description of this view.'),
      'field' => [
        'id' => 'date',
      ],
      'sort' => [
        'id' => 'date',
      ],
      'filter' => [
        'id' => 'date',
      ],
    ];

    $data['tripal_mviews']['mview_edit_link'] = [
      'title' => $this->t('Edit Materialized View'),
      'help' => $this->t('Clickable link to edit a materialized view'),
      'field' => [
        'id' => 'chado_mviews_edit_link',
      ],
    ];
    $data['tripal_mviews']['mview_populate_link'] = [
      'title' => $this->t('Populate Materialized View'),
      'help' => $this->t('Clickable link to populate a materialized view'),
      'field' => [
        'id' => 'chado_mviews_populate_link',
      ],
    ];
    $data['tripal_mviews']['mview_delete_link'] = [
      'title' => $this->t('Delete Materialized View'),
      'help' => $this->t('Clickable link to delete a materialied view'),
      'field' => [
        'id' => 'chado_mviews_delete_link',
      ],
    ];
  }

  /**
   * Describes the db, cv, dbxref, and cvterm tables for Drupal views.
   *
   * We include a relationship so that we can make the double-hop
   * from cvterm, through dbxref, to the db table.
   *
   * To access chado tables, we need a chado connection. This is provided
   * by specifying the 'query_id' to point to our views connection class.
   *
   * @param array &$data
   *   Description to pass to hook_views_data.
   */
  protected function viewsDataTermTables(&$data) {
    // The chado db table.
    $data['db']['table']['group'] = $this->t('External Databases');
    $data['db']['table']['base'] = [
      'field' => 'db_id',
      'title' => $this->t('Chado table: db'),
      'help' => $this->t('External databases'),
      'query_id' => 'tripal_chado_views_query',
    ];
    $data['db']['table']['join']['dbxref'] = [
      'left_field' => 'db_id',
      'field' => 'db_id',
    ];
    $data['db']['db_id'] = [
      'title' => $this->t('Primary key'),
      'help' => $this->t('The primary key value associated with this row'),
      'sort' => ['id' => 'standard'],
      'field' => ['id' => 'standard'],
      'filter' => ['id' => 'numeric'],
    ];
    $data['db']['name'] = [
      'title' => $this->t('Database Name'),
      'help' => $this->t('The name of the external database'),
      'sort' => ['id' => 'standard'],
      'field' => ['id' => 'standard'],
      'filter' => ['id' => 'string'],
    ];
    $data['db']['description'] = [
      'title' => $this->t('Database Description'),
      'help' => $this->t('A short description of the external database'),
      'sort' => ['id' => 'standard'],
      'field' => ['id' => 'standard'],
      'filter' => ['id' => 'string'],
    ];
    $data['db']['urlprefix'] = [
      'title' => $this->t('URL Prefix'),
      'help' => $this->t('URL prefix to allow linking to accessions within the external database'),
      'sort' => ['id' => 'standard'],
      'field' => ['id' => 'standard'],
      'filter' => ['id' => 'string'],
    ];
    $data['db']['url'] = [
      'title' => $this->t('URL'),
      'help' => $this->t('URL linking to primary reference of the external database'),
      'sort' => ['id' => 'standard'],
      'field' => ['id' => 'standard'],
      'filter' => ['id' => 'string'],
    ];
    $data['db']['db_edit_link'] = [
      'title' => $this->t('Edit Database'),
      'help' => $this->t('Clickable link to edit an external database'),
      'field' => [
        'id' => 'chado_db_edit_link',
      ],
    ];
    $data['db']['db_delete_link'] = [
      'title' => $this->t('Delete Database'),
      'help' => $this->t('Clickable link to delete an external database'),
      'field' => [
        'id' => 'chado_db_delete_link',
      ],
    ];
    $data['db']['db_items_link'] = [
      'title' => $this->t('Cross Reference Items'),
      'help' => $this->t('Clickable link to view cross references in a single database'),
      'field' => [
        'id' => 'chado_db_items_link',
      ],
    ];

    // The chado cv table.
    $data['cv']['table']['group'] = $this->t('Controlled Vocabularies');
    $data['cv']['table']['base'] = [
      'field' => 'cv_id',
      'title' => $this->t('Chado table: cv'),
      'help' => $this->t('Controlled vocabularies'),
      'query_id' => 'tripal_chado_views_query',
    ];
    $data['cv']['table']['join']['cvterm'] = [
      'left_field' => 'cv_id',
      'field' => 'cv_id',
    ];
    $data['cv']['cv_id'] = [
      'title' => $this->t('Primary key'),
      'help' => $this->t('The primary key value associated with this row'),
      'field' => ['id' => 'standard'],
      'sort' => ['id' => 'standard'],
      'filter' => ['id' => 'numeric'],
    ];
    $data['cv']['name'] = [
      'title' => $this->t('Controlled Vocabulary Name'),
      'help' => $this->t('The name of the controlled vocabulary'),
      'sort' => ['id' => 'standard'],
      'field' => ['id' => 'standard'],
      'filter' => ['id' => 'string'],
    ];
    $data['cv']['definition'] = [
      'title' => $this->t('Controlled vocabulary definition'),
      'help' => $this->t('A short description of the controlled vocabulary'),
      'sort' => ['id' => 'standard'],
      'field' => ['id' => 'standard'],
      'filter' => ['id' => 'string'],
    ];
    $data['cv']['cv_edit_link'] = [
      'title' => $this->t('Edit Vocabulary'),
      'help' => $this->t('Clickable link to edit a controlled vocabulary'),
      'field' => [
        'id' => 'chado_cv_edit_link',
      ],
    ];
    $data['cv']['cv_delete_link'] = [
      'title' => $this->t('Delete Vocabulary'),
      'help' => $this->t('Clickable link to delete a controlled vocabulary'),
      'field' => [
        'id' => 'chado_cv_delete_link',
      ],
    ];
    $data['cv']['cv_items_link'] = [
      'title' => $this->t('Controlled Vocabulary Items'),
      'help' => $this->t('Clickable link to view vocabulary terms in a single controlled vocabulary'),
      'field' => [
        'id' => 'chado_cv_items_link',
      ],
    ];

    // The chado dbxref table.
    $data['dbxref']['table']['group'] = $this->t('Database cross-reference');
    $data['dbxref']['table']['base'] = [
      'field' => 'dbxref_id',
      'title' => $this->t('Chado table: dbxref'),
      'help' => $this->t('Database cross-reference'),
      'query_id' => 'tripal_chado_views_query',
    ];
    $data['dbxref']['table']['join']['cvterm'] = [
      'left_field' => 'dbxref_id',
      'field' => 'dbxref_id',
    ];
    $data['dbxref']['dbxref_id'] = [
      'title' => $this->t('Primary key'),
      'help' => $this->t('The primary key value associated with this row'),
      'field' => ['id' => 'standard'],
      'sort' => ['id' => 'standard'],
      'filter' => ['id' => 'numeric'],
    ];
    $data['dbxref']['db_id'] = [
      'title' => $this->t('External database ID'),
      'help' => $this->t('The external database that this accession references'),
      'field' => ['id' => 'standard'],
      'sort' => ['id' => 'standard'],
      'filter' => ['id' => 'numeric'],
    ];
    $data['dbxref']['accession'] = [
      'title' => $this->t('Accession'),
      'help' => $this->t('The accession identifier within the external database'),
      'sort' => ['id' => 'standard'],
      'field' => ['id' => 'standard'],
      'filter' => ['id' => 'string'],
    ];
    $data['dbxref']['version'] = [
      'title' => $this->t('Version'),
      'help' => $this->t('The version of this database cross-reference, if applicable'),
      'sort' => ['id' => 'standard'],
      'field' => ['id' => 'standard'],
      'filter' => ['id' => 'string'],
    ];
    $data['dbxref']['dbxref_edit_link'] = [
      'title' => $this->t('Edit Database Cross Reference'),
      'help' => $this->t('Clickable link to edit a cross reference to an external database'),
      'field' => [
        'id' => 'chado_dbxref_edit_link',
      ],
    ];
    $data['dbxref']['dbxref_delete_link'] = [
      'title' => $this->t('Delete Database Cross Reference'),
      'help' => $this->t('Clickable link to delete a cross reference to an external database'),
      'field' => [
        'id' => 'chado_dbxref_delete_link',
      ],
    ];

    // Relationship from cvterm, through dbxref, to db table.
    $data['dbxref']['rel'] = [
      'title' => $this->t('External Database'),
      'help' => $this->t('Relationship through the dbxref table to the db table'),
      'relationship' => [
        'base' => 'db',
        'base field' => 'db_id',
        'field' => 'db_id',
        'id' => 'standard',
        'label' => 'dbxref to db relationship',
      ],
    ];
    $data['dbxref']['dbxref_urlprefix_link'] = [
      'title' => $this->t('External Link'),
      'help' => $this->t('Clickable link to the accession in its host database'),
      'field' => [
        'id' => 'chado_dbxref_urlprefix_link',
      ],
    ];

    // The chado cvterm table.
    $data['cvterm']['table']['group'] = $this->t('Controlled Vocabulary Terms');
    $data['cvterm']['table']['base'] = [
      'field' => 'cvterm_id',
      'title' => $this->t('Chado table: cvterm'),
      'help' => $this->t('Controlled vocabulary terms'),
      'query_id' => 'tripal_chado_views_query',
    ];
    $data['cvterm']['table']['join']['dbxref'] = [
      'left_field' => 'dbxref_id',
      'field' => 'dbxref_id',
    ];
    $data['cvterm']['cvterm_id'] = [
      'title' => $this->t('Primary key'),
      'help' => $this->t('The primary key value associated with this row'),
      'field' => ['id' => 'standard'],
      'sort' => ['id' => 'standard'],
      'filter' => ['id' => 'numeric'],
    ];
    $data['cvterm']['name'] = [
      'title' => $this->t('Controlled Vocabulary Term Name'),
      'help' => $this->t('The name of the controlled vocabulary term'),
      'sort' => ['id' => 'standard'],
      'field' => ['id' => 'standard'],
      'filter' => ['id' => 'string'],
    ];
    $data['cvterm']['definition'] = [
      'title' => $this->t('Controlled vocabulary term definition'),
      'help' => $this->t('The definition of the controlled vocabulary term'),
      'sort' => ['id' => 'standard'],
      'field' => ['id' => 'standard'],
      'filter' => ['id' => 'string'],
    ];
    $data['cvterm']['dbxref_id'] = [
      'title' => $this->t('Database cross-reference'),
      'help' => $this->t('A reference to an external database and accession'),
      'field' => ['id' => 'standard'],
      'sort' => ['id' => 'standard'],
      'filter' => ['id' => 'numeric'],
    ];
    $data['cvterm']['is_obsolete'] = [
      'title' => $this->t('Is obsolete'),
      'help' => $this->t('Indicates if this term is obsolete'),
      'field' => ['id' => 'standard'],
      'sort' => ['id' => 'standard'],
      'filter' => ['id' => 'numeric'],
    ];
    $data['cvterm']['is_relationshiptype'] = [
      'title' => $this->t('Is relationship type'),
      'help' => $this->t('Indicates if this term describes a relationship'),
      'field' => ['id' => 'standard'],
      'sort' => ['id' => 'standard'],
      'filter' => ['id' => 'numeric'],
    ];

    $data['cvterm']['cvterm_edit_link'] = [
      'title' => $this->t('Edit Vocabulary Term'),
      'help' => $this->t('Clickable link to edit a controlled vocabulary term'),
      'field' => [
        'id' => 'chado_cvterm_edit_link',
      ],
    ];
    $data['cvterm']['cvterm_delete_link'] = [
      'title' => $this->t('Delete Vocabulary Term'),
      'help' => $this->t('Clickable link to delete a controlled vocabulary term'),
      'field' => [
        'id' => 'chado_cvterm_delete_link',
      ],
    ];
  }

}
