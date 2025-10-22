<?php

namespace Drupal\tripal\Access;

use Drupal\Core\DependencyInjection\AutowireTrait;
use Drupal\Core\DependencyInjection\ContainerInjectionInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Entity\BundlePermissionHandlerTrait;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Provides dynamic permissions for tripal content of different types.
 */
class TripalEntityPermissions implements ContainerInjectionInterface {

  use AutowireTrait;
  use BundlePermissionHandlerTrait;
  use StringTranslationTrait;

  public function __construct(
    protected ?EntityTypeManagerInterface $entityTypeManager = NULL,
  ) {
    if ($entityTypeManager === NULL) {
      @trigger_error('Calling ' . __METHOD__ . ' without the $entityTypeManager argument is deprecated in drupal:11.2.0 and it will be required in drupal:12.0.0. See https://www.drupal.org/node/3515921', E_USER_DEPRECATED);
      $this->entityTypeManager = \Drupal::entityTypeManager();
    }
  }

  /**
   * Returns an array of content type permissions.
   *
   * @return array
   *   The content type permissions.
   *
   * @see \Drupal\user\PermissionHandlerInterface::getPermissions()
   */
  public function tripalContentTypePermissions() {
    return $this->generatePermissions(
      $this->entityTypeManager->getStorage('tripal_entity_type')->loadMultiple(),
      [$this, 'buildPermissions']
    );
  }

  /**
   * Returns a list of content permissions for a given content type.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $type
   *   The content type.
   *
   * @return array
   *   An associative array of permission names and descriptions.
   */
  protected function buildPermissions(TripalEntityType $type) {
    $type_id = $type->id();
    $type_params = ['%type_name' => $type->label()];

    return [
      "create $type_id content" => [
        'title' => $this->t('%type_name: Create new content', $type_params),
      ],
      "edit own $type_id content" => [
        'title' => $this->t('%type_name: Edit own content', $type_params),
        'description' => $this->t('Note that anonymous users with this permission are able to edit any content created by any anonymous user.'),
      ],
      "edit any $type_id content" => [
        'title' => $this->t('%type_name: Edit any content', $type_params),
      ],
      "unpublish own $type_id content" => [
        'title' => $this->t('%type_name: Unpublish own content', $type_params),
        'description' => $this->t('Note that anonymous users with this permission are able to delete any content created by any anonymous user.'),
      ],
      "unpublish any $type_id content" => [
        'title' => $this->t('%type_name: Unpublish any content', $type_params),
      ],
      "delete own $type_id content" => [
        'title' => $this->t('%type_name: Delete own content', $type_params),
        'description' => $this->t('Note that anonymous users with this permission are able to delete any content created by any anonymous user.'),
      ],
      "delete any $type_id content" => [
        'title' => $this->t('%type_name: Delete any content', $type_params),
      ],
      "view all $type_id content" => [
        'title' => $this->t('%type_name: View all content', $type_params),
      ],
      "view own $type_id content" => [
        'title' => $this->t('%type_name: View own content', $type_params),
        'description' => $this->t('Note that anonymous users with this permission are able to view any content created by any anonymous user.'),
      ],
    ];
  }

}
