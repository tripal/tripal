<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Url;

/**
 * Controller routines related to Tripal Entity and Tripal Entity Type UIs.
 */
class TripalEntityUIController extends ControllerBase {

  /**
   * Constructs the TripalEntityUIController.
   */
  public function __construct() {
  }

  /**
   * The Tripal Content Add page where content types are listed.
   *
   * Route: entity.tripal_entity.add_page
   * Template: tripal-entity-content-add-list.html.twig
   *
   * @return Drupal\Core\Render\Element
   *   Returns a rendered listing of Tripal Content Types linking to add forms.
   */
  public function tripalContentAddPage() {

    // Get a list of all types.
    $bundle_entities = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties([]);

    // Now compile them into variables to be used in twig.
    $bundles = [];
    foreach ($bundle_entities as $entity) {
      $category = $entity->getCategory();
      $bundles[$category]['title'] = $category;
      $bundles[$category]['members'][] = [
        'title' => $entity->getLabel(),
        'help' => $entity->getHelpText(),
        'url' => Url::fromRoute('entity.tripal_entity.add_form', ['tripal_entity_type' => $entity->getName()]),
      ];
    }

    // Finally, let tripal-entity-content-add-list.html.twig add the markup.
    return [
      '#theme' => 'tripal_entity_content_add_list',
      '#types' => $bundles,
    ];
  }

  /**
   * Checks for to see if new fields need to be added to a Tripal Content Type.
   *
   * @todo call this from somewhere.
   * @todo update all code.
   */
  public function tripalCheckForFields($tripal_entity_type) {

    $bundle_name = $tripal_entity_type->getName();
    $term = $tripal_entity_type->getTerm();

    //$added = tripal_create_bundle_fields($bundle, $term);
    //if (count($added) == 0) {
      //drupal_set_message('No new fields were added');
    //}
    //foreach ($added as $field_name) {
      //drupal_set_message('Added field: ' . $field_name);
    //}

    \Drupal::messenger()->addWarning(t('This functionality is not implemented yet.'));

    return $this->redirect('entity.tripal_entity.field_ui_fields',
      ['tripal_entity_type' => $bundle_name]);
  }
}
