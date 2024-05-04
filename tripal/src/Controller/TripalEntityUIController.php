<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Url;
use Drupal\Core\Link;

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
        'url' => Url::fromRoute('entity.tripal_entity.add_form', ['tripal_entity_type' => $entity->id()]),
      ];
    }

    // If there are no tripal content types / bundles
    if (count($bundle_entities) <= 0) {
      //$url_vocab_management = Url::fromRoute('entity.tripal_vocab.collection');
      //$link = Link::fromTextAndUrl('creating a vocabulary',
      //          $url_vocab_management)->toString();

      $url_type_management = Url::fromRoute('entity.tripal_entity_type.add_form');
      $link = Link::fromTextAndUrl('creating one',
                $url_type_management)->toString();

      // Because this message contains a link, we need to render it before
      // displaying it using the messenger.
      $message = 'There are currently no Tripal Content Types, ' .
                 'please begin by ' . $link . '.';
      $rendered_message = \Drupal\Core\Render\Markup::create($message);

      // Display the message to create a vocabulary
      $messenger = \Drupal::messenger();
      $messenger->addMessage($rendered_message,'warning');
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

    $bundle_name = $tripal_entity_type->id();

    $messenger = \Drupal::messenger();

    // Get all of the fields and call the `discover()` method for each one.
    /** @var \Drupal\Core\Field\FieldTypePluginManager $field_type_manager **/
    $field_type_manager = \Drupal::service('plugin.manager.field.field_type');
    /** @var \Drupal\Core\Entity\EntityFieldManager $entity_field_manager **/
    $entity_field_manager = \Drupal::service('entity_field.manager');
    /** @var \Drupal\tripal\Services\TripalFieldCollection $tripal_fields **/
    $tripal_fields = \Drupal::service('tripal.tripalfield_collection');

    $all_field_defs = $field_type_manager->getDefinitions();
    $entity_field_defs = $entity_field_manager->getFieldDefinitions('tripal_entity', $bundle_name);
    foreach ($all_field_defs as $field_id => $field_def) {
      $field_class = $field_def['class'];
      if (is_subclass_of($field_class, 'Drupal\tripal\TripalField\TripalFieldItemBase')) {
        $discovered = $field_class::discover($tripal_entity_type, $field_id, $all_field_defs);
        foreach ($discovered as $discovered_field) {

          // If the doscovered field already exists then skip it.
          if (array_key_exists($discovered_field['name'], $entity_field_defs)) {
            $messenger->addStatus('Skipping field, "' . $discovered_field['name'] .'", as it already exists for this content type.');
            continue;
          }

          // If the field is not valid then skip it.
          $is_valid = $tripal_fields->validate($discovered_field);
          if (!$is_valid) {
            throw new \Exception('Error: "' . $discovered_field['name'] . '", as it did not pass validation checks.');
          }

          // Add the field
          $added = $tripal_fields->addBundleField($discovered_field);
          if ($added) {
            $messenger->addMessage('Successfully added field "' . $discovered_field['name'] . '"');
          }
          else {
            $messenger->addError('Could not add field, "' . $discovered_field['name'] . '". Check the Drupal logs for more information.');
          }
        }
      }
    }

    return $this->redirect('entity.tripal_entity.field_ui_fields',
      ['tripal_entity_type' => $bundle_name]);
  }
}
