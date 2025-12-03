<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Render\Markup;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Url;
use Drupal\Core\Link;
use Drupal\tripal\Access\TripalEntityAccessControlHandler;

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
   * @see entity.tripal_entity.add_page
   * @see tripal-entity-content-add-list.html.twig
   *
   * @return \Drupal\Core\Render\Element
   *   Returns a rendered listing of Tripal Content Types linking to add forms.
   */
  public function tripalContentAddPage() {

    // Get the content types that the user has create access for.
    $bundle_entities = TripalEntityAccessControlHandler::checkCreateAccessForTypes($this->currentUser());

    // Sort the entities using the entity class's sort() method.
    // See \Drupal\Core\Config\Entity\ConfigEntityBase::sort().
    uasort($bundle_entities, [
      TripalEntityType::class,
      'sortByCategory',
    ]);

    // Now compile them into variables to be used in twig.
    $bundles = [];
    foreach ($bundle_entities as $entity) {
      $category = $entity->getCategory();
      $bundles[$category]['title'] = $category;
      $bundles[$category]['class'] = str_replace(' ', '', $category);
      $bundles[$category]['members'][] = [
        'title' => $entity->getLabel(),
        'help' => $entity->getHelpText(),
        'id' => $entity->id(),
        'url' => Url::fromRoute('entity.tripal_entity.add_form', ['tripal_entity_type' => $entity->id()]),
      ];
    }

    // If there are no tripal content types / bundles.
    if (count($bundle_entities) <= 0) {
      $url_type_management = Url::fromRoute('entity.tripal_entity_type.add_form');
      $link = Link::fromTextAndUrl('creating one',
                $url_type_management)->toString();

      // Because this message contains a link, we need to render it before
      // displaying it using the messenger.
      $message = 'There are currently no Tripal Content Types, please begin by ' . $link . '.';
      $rendered_message = Markup::create($message);

      // Display the message to create a vocabulary.
      $this->messenger()->addMessage($rendered_message, 'warning');
    }

    // Finally, let tripal-entity-content-add-list.html.twig add the markup.
    return [
      '#theme' => 'tripal_entity_content_add_list',
      '#attached' => [
        'library' => [
          'tripal/tripal-entity-add-content',
        ],
      ],
      '#types' => $bundles,
    ];
  }

}
