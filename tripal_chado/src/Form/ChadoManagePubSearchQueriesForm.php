<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;


class ChadoManagePubSearchQueriesForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_manage_pub_search_queries_form';
  }

  /**
   * {@inheritDoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    // // Check to make sure that the tripal_pub vocabulary is loaded. If not, then
  // // warn the user that they should load it before continuing.
  // $pub_cv = chado_select_record('cv', ['cv_id'], ['name' => 'tripal_pub']);
  // if (count($pub_cv) == 0) {
  //   drupal_set_message(t('The Tripal Pub vocabulary is currently not loaded. ' .
  //     'This vocabulary is required to be loaded before importing of ' .
  //     'publications.  <br>Please !import',
  //     ['!import' => l('load the Tripal Publication vocabulary', 'admin/tripal/loaders/chado_vocabs/obo_loader')]), 'warning');
  // }

  // // clear out the session variable when we view the list.
  // unset($_SESSION['tripal_pub_import']);

  // $headers = [
  //   '',
  //   'Importer Name',
  //   'Database',
  //   'Search String',
  //   'Disabled',
  //   'Create Contact',
  //   '',
  // ];
  // $rows = [];
  // $importers = db_query("SELECT * FROM {tripal_pub_import} ORDER BY name");

  // while ($importer = $importers->fetchObject()) {
  //   $criteria = unserialize($importer->criteria);
  //   $num_criteria = $criteria['num_criteria'];
  //   $criteria_str = '';
  //   for ($i = 1; $i <= $num_criteria; $i++) {
  //     $search_terms = $criteria['criteria'][$i]['search_terms'];
  //     $scope = $criteria['criteria'][$i]['scope'];
  //     $is_phrase = $criteria['criteria'][$i]['is_phrase'];
  //     $operation = $criteria['criteria'][$i]['operation'];
  //     $criteria_str .= "$operation ($scope: $search_terms) ";
  //   }

  //   $rows[] = [
  //     [
  //       'data' => l(t('Edit/Test'), "admin/tripal/loaders/pub/edit/$importer->pub_import_id") . '<br>' .
  //         l(t('Import Pubs'), "admin/tripal/loaders/pub/submit/$importer->pub_import_id"),
  //       'nowrap' => 'nowrap',
  //     ],
  //     $importer->name,
  //     $criteria['remote_db'],
  //     $criteria_str,
  //     $importer->disabled ? 'Yes' : 'No',
  //     $importer->do_contact ? 'Yes' : 'No',
  //     l(t('Delete'), "admin/tripal/loaders/pub/delete/$importer->pub_import_id"),
  //   ];
  // }

  // The link to add a new publication
  $html = "<ul class='action-links'>";
  $html .= '  <li>' . Link::fromTextAndUrl('New Importer', Url::fromUri('internal:/admin/tripal/loaders/publications/new_publication'))->toString() . '</li>';
  $html .= '</ul>';
  $form['new_publication_link'] = [
    '#type' => 'markup',
    '#markup' => $html
  ];
  unset($html); 

  $html = '<p>' . t(
      "A publication importer is used to create a set of search criteria that can be used
     to query a remote database, find publications that match the specified criteria
     and then import those publications into the Chado database. An example use case would
     be to peridocially add new publications to this Tripal site that have appeared in PubMed
     in the last 30 days.  You can import publications in one of two ways:
     <ol>
      <li>Create a new importer by clicking the 'New Importer' link above, and after saving it should appear in the list below.  Click the
          link labeled 'Import Pubs' to schedule a job to import the publications</li>
      <li>The first method only performs the import once.  However, you can schedule the
          importer to run periodically by adding a cron job. </li>
     </ol><br>");
  $form['heading'] = [
    '#type' => 'markup',
    '#markup' => $html
  ];

  $headers = [
    '',
    'Importer Name',
    'Database',
    'Search String',
    'Disabled',
    'Create Contact',
    '',
  ];
  $form['pub_manager']['table'] = [
    '#type' => 'table',
    '#header' => $headers,
    '#prefix' => '<div id="pub_manager_table">',
    '#suffix' => '</div>',
    '#weight' => 50,
  ];

  $public = \Drupal::database();
  $pub_importers_query = $public->select('tripal_pub_import');
  $pub_importers_count= $pub_importers_query->countQuery()->execute()->fetchField();
  if ($pub_importers_count > 0) {
    // Lookup all records
    $pub_importers = $pub_importers_query->execute();
    foreach ($pub_importers as $pub_importer) {
      dpm($pub_importer);
    }
  }
  else {
    // No publication importers have been added by users
    $row['col-1'] = [
      '#type' => 'markup',
      '#markup' => ''
    ];
    $row['col-2'] = [
      '#type' => 'markup',
      '#markup' => 'There are currently no importers'
    ];
    $row['col-3'] = [
      '#type' => 'markup',
      '#markup' => ''
    ];
    $row['col-4'] = [
      '#type' => 'markup',
      '#markup' => ''
    ];
    $row['col-5'] = [
      '#type' => 'markup',
      '#markup' => ''
    ];
    $row['col-6'] = [
      '#type' => 'markup',
      '#markup' => ''
    ]; 
    $row['col-7'] = [
      '#type' => 'markup',
      '#markup' => ''
    ];       
    $form['pub_manager']['table']['no_publications'] = $row;
  }


  // $page .= '<p>' . t(
  //     "A publication importer is used to create a set of search criteria that can be used
  //    to query a remote database, find publications that match the specified criteria
  //    and then import those publications into the Chado database. An example use case would
  //    be to peridocially add new publications to this Tripal site that have appeared in PubMed
  //    in the last 30 days.  You can import publications in one of two ways:
  //    <ol>
  //     <li>Create a new importer by clicking the 'New Importer' link above, and after saving it should appear in the list below.  Click the
  //         link labeled 'Import Pubs' to schedule a job to import the publications</li>
  //     <li>The first method only performs the import once.  However, you can schedule the
  //         importer to run periodically by adding a cron job. </li>
  //    </ol><br>");

  // $form = drupal_get_form('tripal_pub_importer_ncbi_api_key_form');
  // $page .= drupal_render($form);

  // $table = [
  //   'header' => $headers,
  //   'rows' => $rows,
  //   'attributes' => [
  //   ],
  //   'caption' => '',
  //   'sticky' => TRUE,
  //   'colgroups' => [],
  //   'empty' => 'There are currently no importers',
  // ];

  // $page .= theme_table($table);

  // return $page;

    return $form;
  }




  /**
   * {@inheritDoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
  }

}