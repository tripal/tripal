<?php

namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\tripal\TripalPubParser\Interfaces\TripalPubParserInterface;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Render\Markup;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;

// cf. src/Entity/ChadoTermMapping.php
/**
 * ChadoPubImporter implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "chado_pub_loader",
 *    label = @Translation("Chado Bulk Publication Importer"),
 *    description = @Translation("Create and modify importers that can connect to and retrieve publications from remote databases or local files."),
 *    button_text = @Translation("Add New Loader"),
 *    use_analysis = False,
 *    require_analysis = False,
 *    file_upload = False,
 *    file_load = False,
 *    file_remote = False,
 *    file_required = False,
 *    cardinality = 1,
 *    menu_path = "",
 *    callback = "",
 *    callback_module = "",
 *    callback_path = "",
 *  )
 */
class ChadoPubImporter extends ChadoImporterBase {

  /**
   * The name of this loader. This name will be presented to the site
   * user.
   */
  public static $name = 'Chado Publication Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_pub_loader';

  /**
   * A brief description for this loader. This description will be
   * presented to the site user.
   */
  public static $description = 'Import publications into Chado';

  /**
   * {@inheritDoc}
   */
  public function form($form, &$form_state) {
    // Always call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);

    $form = $this->tripal_pub_importers_list($form, $form_state);

    return $form;
  }

  /**
   * {@inheritDoc}
   */
  public function formSubmit($form, &$form_state) {
    $trigger = $form_state->getTriggeringElement()['#name'];
    // 'op' is default submit; 'add_new_loader' we have defined for second submit
//    if ($trigger == 'add_new_loader') {
      $form_state->setRedirect('/admin/tripal/loaders/chado_pub_loader_edit');
      return;
//    }
  }

  /**
   * {@inheritDoc}
   */
  public function postRun() {
  }

  /**
   * {@inheritDoc}
   */
  public function formValidate($form, &$form_state) {
  }

  /**
   * {@inheritDoc}
   */
  public function run() {
  }

  /**
   * A function to generate a table containing the list of publication importers
   *
   * @ingroup tripal_pub
   */
  function tripal_pub_importers_list($form, &$form_state) {

    // Check to make sure that the tripal_pub vocabulary is loaded. If not, then
    // warn the user that they should load it before continuing.
$chado = \Drupal::service('tripal_chado.database');
//    $chado = $this->getChadoConnection();
    $query = $chado->select('cv')
      ->condition('name', 'tripal_pub', '=');
    $count = $query->countQuery()->execute()->fetchField();
    if (!$count) {
      \Drupal::messenger()->addWarning(t('The Tripal Pub vocabulary is currently not loaded. ' .
        'This vocabulary is required to be loaded before importing of ' .
        'publications.  <br>Please !import',
        ['!import' => Link::fromTextAndUrl('load the Tripal Publication vocabulary',
          Url::fromUri('internal:/admin/tripal/loaders/chado_vocabs/obo_loader'))->toString()])
      );
    }

// tv3 had this
//  // clear out the session variable when we view the list.
//  unset($_SESSION['tripal_pub_import']);

    $headers = [
      '',
      'Importer Name',
      'Database',
      'Search String',
      'Disabled',
      'Create Contact',
      '',
    ];
    $rows = [];

    $public = \Drupal::service('database');
    $query = $public->select('tripal_pub_import', 'I')
      ->fields('I')
      ->orderBy('I.name', 'ASC');
    $importers = $query->execute();

    while ($importer = $importers->fetchObject()) {
      $criteria = unserialize($importer->criteria);
      $num_criteria = $criteria['num_criteria'];
      $criteria_str = '';
      for ($i = 1; $i <= $num_criteria; $i++) {
        $search_terms = $criteria['criteria'][$i]['search_terms'];
        $scope = $criteria['criteria'][$i]['scope'];
        $is_phrase = $criteria['criteria'][$i]['is_phrase'];
        $operation = $criteria['criteria'][$i]['operation'];
        $criteria_str .= "$operation ($scope: $search_terms) ";
      }

      $rows[] = [
        [
          'data' => Link::fromTextAndUrl(t('Edit/Test'), Url::fromUri("internal:/admin/tripal/loaders/pub/edit/$importer->pub_import_id"))->toString() . '<br>' .
            Link::fromTextAndUrl(t('Import Pubs'), Url::fromUri("internal:/admin/tripal/loaders/pub/submit/$importer->pub_import_id"))->toString(),
          'nowrap' => 'nowrap',
        ],
        $importer->name,
        $criteria['remote_db'],
        $criteria_str,
        $importer->disabled ? 'Yes' : 'No',
        $importer->do_contact ? 'Yes' : 'No',
        Link::fromTextAndUrl(t('Delete'), Url::fromUri("internal:/admin/tripal/loaders/pub/delete/$importer->pub_import_id"))->toString(),
      ];
    }

    // to-do this is from tripal 3 version, should it be modified?
    $page = '<p>' . t(
        "A publication importer is used to create a set of search criteria that can be used
       to query a remote database, find publications that match the specified criteria
       and then import those publications into the Chado database. An example use case would
       be to peridocially add new publications to this Tripal site that have appeared in PubMed
       in the last 30 days.  You can import publications in one of two ways:
       <ol>
        <li>Create a new importer by clicking the 'Add New Importer' button below, and after
            saving it should appear in the list below.
            Click the link labeled 'Import Pubs' to schedule a job to import the publications</li>
        <li>The first method only performs the import once.  However, you can schedule the
            importer to run periodically by adding a cron job. </li>
       </ol><br>");

    $form['loaders']['description'] = [
      '#type' => 'markup',
      '#markup' => $page,
    ];
    $form['loaders']['table'] = [
      '#type' => 'tableselect',
      '#header' => $headers,
      '#options' => $rows,
      '#sticky' => TRUE,
      '#empty' => 'There are currently no importers',
    ];

    return $form;
  }

}
