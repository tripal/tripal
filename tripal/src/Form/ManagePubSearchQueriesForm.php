<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;


class ManagePubSearchQueriesForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_manage_publication_search_queries_form';
  }

  /**
   * {@inheritDoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    // The link to add a new publication
    $html = "<ul class='action-links'>";
    $html .= '  <li>' . Link::fromTextAndUrl('New search query', Url::fromUri('internal:/admin/tripal/loaders/publications/new_publication_search_query'))->toString() . '</li>';
    $html .= '</ul>';
    $form['new_publication_link'] = [
      '#markup' => $html
    ];
    unset($html); 

    $html = '<p>' . t(
        "A publication importer is used to create a set of search criteria that can be used
      to query a remote database, find publications that match the specified criteria
      and then import those publications into the Chado database. An example use case would
      be to periodically add new publications to this Tripal site that have appeared in PubMed
      in the last 30 days.  You can import publications in one of two ways:
      <ol>
        <li>Create a new importer by clicking the 'New Importer' link above, and after saving it should appear in the list below.  Click the
            link labeled 'Import Pubs' to schedule a job to import the publications</li>
        <li>The first method only performs the import once.  However, you can schedule the
            importer to run periodically by adding a cron job. </li>
      </ol><br>");
    $form['heading'] = [
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
      'Actions',
    ];
    $form['pub_manager']['table'] = [
      '#type' => 'table',
      '#header' => $headers,
      '#prefix' => '<div id="pub_manager_table">',
      '#suffix' => '</div>',
      '#weight' => 50,
    ];

    $public = \Drupal::database();
    $pub_importers_query = $public->select('tripal_pub_library_query','tpi');
    $pub_importers_count = $pub_importers_query->countQuery()->execute()->fetchField();

    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $pub_queries = $pub_library_manager->getSearchQueries();
    $pub_queries_count = count($pub_queries);

    if ($pub_queries_count > 0) {

      foreach ($pub_queries as $pub_query) {
        $criteria_column_array = unserialize($pub_query->criteria);

        $search_string = "";
        foreach ($criteria_column_array['criteria'] as $criteria_row) {
          $search_string .= $criteria_row['operation'] . ' (' . $criteria_row['scope'] . ': ' . $criteria_row['search_terms'] . ') ';
        }

        $disabled = $criteria_column_array['disabled'];
        if ($disabled <= 0) {
          $disabled = 'No';
        }
        else {
          $disabled = 'Yes';
        }

        $do_contact = $criteria_column_array['do_contact'];
        if ($do_contact <= 0) {
          $do_contact = 'No';
        }
        else {
          $do_contact = 'Yes';
        }

        $row = [];

        // This should contain edit test and import pubs links @TODO
        $row['col-1'] = [
          '#markup' => 
            Link::fromTextAndUrl(
              'Edit/Test', 
              Url::fromUri('internal:/admin/tripal/loaders/publications/edit_publication_search_query/' . $pub_query->pub_library_query_id)
            )
            ->toString() . 
            '<br /><a href="">Import Pubs</a>'
        ];
        $row['col-2'] = [
          '#markup' => $pub_query->name
        ];
        $row['col-3'] = [
          '#markup' => $criteria_column_array['remote_db']
        ];

        // Search string
        $row['col-4'] = [
          '#markup' => $search_string
        ];

        // Disabled
        $row['col-5'] = [
          '#markup' => $disabled
        ];

        // Create contact
        $row['col-6'] = [
          '#markup' => $do_contact
        ];




        // Delete should be a button instead of markup @TODO
        $row['col-7-delete-' . $pub_query->pub_library_query_id] = [
          '#type' => 'submit',
          '#name' => 'delete-' . $pub_query->pub_library_query_id,
          '#default_value' => 'Delete',
        ];

        // Actions
        $row['col-8'] = [
          '#markup' => '
            <div class="dropbutton-wrapper dropbutton-multiple" data-drupal-ajax-container="" data-once="dropbutton">
              <div class="dropbutton-widget">
                <ul class="dropbutton dropbutton--multiple">
                <li class="mview-edit-link dropbutton__item dropbutton-action">
                  <a href="/admin/tripal/storage/chado/mview/4">Edit</a>
                </li>
                <li class="dropbutton-toggle">
                  <button type="button" class="dropbutton__toggle">
                  <span class="visually-hidden">List additional actions</span>
                  </button>
                </li>
                <li class="mview-populate-link dropbutton__item dropbutton-action secondary-action">
                  <a href="/admin/tripal/storage/chado/mview_populate/4">Populate</a>
                </li>
                <li class="mview-delete-link dropbutton__item dropbutton-action secondary-action">
                  <a href="/admin/tripal/storage/chado/mview_delete/4">Delete</a>
                </li>
                </ul>
              </div></div>
          '
        ];

        $form['pub_manager']['table'][] = $row;
      }
    }
    else {
      // No publication importers have been added by users
      $row['col-1'] = [
        '#markup' => ''
      ];
      $row['col-2'] = [
        '#markup' => 'There are currently no importers'
      ];
      $row['col-3'] = [
        '#markup' => ''
      ];
      $row['col-4'] = [
        '#markup' => ''
      ];
      $row['col-5'] = [
        '#markup' => ''
      ];
      $row['col-6'] = [
        '#markup' => ''
      ]; 
      $row['col-7'] = [
        '#markup' => ''
      ];       
      $form['pub_manager']['table']['no_publications'] = $row;
    }

    return $form;
  }


  /**
   * {@inheritDoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $public = \Drupal::database();
    $user_input = $form_state->getUserInput();
    $trigger_element = $form_state->getTriggeringElement();
    if (stripos($trigger_element['#name'],'delete-') !== FALSE) {
      $pub_library_query_id = explode('delete-',$trigger_element['#name'])[1];
      $url = Url::fromUri('internal:/admin/tripal/loaders/publications/delete_publication_search_query/' . $pub_library_query_id);
      $form_state->setRedirectUrl($url);
    }
  }

}