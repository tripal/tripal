<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Render\Markup;
use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\Core\Entity\EntityViewBuilder;
use Drupal\Core\Entity\Attribute\ContentEntityType;
use Drupal\Core\Entity\Sql\SqlContentEntityStorage;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Component\Utility\Xss;
use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemList;
use Drupal\user\UserInterface;
use Drupal\tripal\Access\TripalEntityAccessControlHandler;
use Drupal\tripal\Form\TripalEntityForm;
use Drupal\tripal\Form\TripalEntityDeleteForm;
use Drupal\tripal\Form\TripalEntityUnpublishForm;
use Drupal\tripal\Routing\TripalEntityHtmlRouteProvider;
use Drupal\tripal\ListBuilders\TripalEntityListBuilder;
use Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface;

/**
 * Defines the Tripal Content entity.
 *
 * @ingroup tripal
 */
#[ContentEntityType(
  id: 'tripal_entity',
  label: new TranslatableMarkup('Tripal Content'),
  bundle_label: new TranslatableMarkup('Tripal Content Type'),
  handlers: [
    'storage' => SqlContentEntityStorage::class,
    'list_builder' => TripalEntityListBuilder::class,
    'view_builder' => EntityViewBuilder::class,
    'views_data' => TripalEntityViewsData::class,
    'form' => [
      'default' => TripalEntityForm::class,
      'add' => TripalEntityForm::class,
      'edit' => TripalEntityForm::class,
      'delete' => TripalEntityDeleteForm::class,
      'unpublish' => TripalEntityUnpublishForm::class,
    ],
    'access' => TripalEntityAccessControlHandler::class,
    'route_provider' => [
      'html' => TripalEntityHtmlRouteProvider::class,
    ],
  ],
  base_table: 'tripal_entity',
  entity_keys: [
    'id' => 'id',
    'bundle' => 'type',
    'uid' => 'user_id',
    'status' => 'status',
  ],
  links: [
    'canonical' => '/bio_data/{tripal_entity}',
    'add-page' => '/bio_data/add',
    'add-form' => '/bio_data/add/{tripal_entity_type}',
    'edit-form' => '/bio_data/{tripal_entity}/edit',
    'delete-form' => '/bio_data/{tripal_entity}/delete',
    'unpublish-form' => '/bio_data/{tripal_entity}/unpublish',
    'collection' => '/admin/content/bio_data',
  ],
  permission_granularity: 'bundle',
  bundle_entity_type: 'tripal_entity_type',
  field_ui_base_route: 'entity.tripal_entity_type.edit_form',
)]
/**
 * Entity defining biological content for Tripal.
 *
 * @todo Remove this annotation when we no longer support Drupal 10.x.
 *
 * @ContentEntityType(
 *   id = "tripal_entity",
 *   label = @Translation("Tripal Content"),
 *   bundle_label = @Translation("Tripal Content type"),
 *   handlers = {
 *     "storage" = "Drupal\Core\Entity\Sql\SqlContentEntityStorage",
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\tripal\ListBuilders\TripalEntityListBuilder",
 *     "views_data" = "Drupal\tripal\Entity\TripalEntityViewsData",
 *
 *     "form" = {
 *       "default" = "Drupal\tripal\Form\TripalEntityForm",
 *       "add" = "Drupal\tripal\Form\TripalEntityForm",
 *       "edit" = "Drupal\tripal\Form\TripalEntityForm",
 *       "delete" = "Drupal\tripal\Form\TripalEntityDeleteForm",
 *       "unpublish" = "Drupal\tripal\Form\TripalEntityUnpublishForm",
 *     },
 *     "access" = "Drupal\tripal\Access\TripalEntityAccessControlHandler",
 *     "route_provider" = {
 *       "html" = "Drupal\tripal\Routing\TripalEntityHtmlRouteProvider",
 *     },
 *   },
 *   base_table = "tripal_entity",
 *   entity_keys = {
 *     "id" = "id",
 *     "bundle" = "type",
 *     "uid" = "user_id",
 *     "status" = "status",
 *   },
 *   links = {
 *     "canonical" = "/bio_data/{tripal_entity}",
 *     "add-page" = "/bio_data/add",
 *     "add-form" = "/bio_data/add/{tripal_entity_type}",
 *     "edit-form" = "/bio_data/{tripal_entity}/edit",
 *     "delete-form" = "/bio_data/{tripal_entity}/delete",
 *     "unpublish-form" = "/bio_data/{tripal_entity}/unpublish",
 *     "collection" = "/admin/content/bio_data",
 *   },
 *   bundle_entity_type = "tripal_entity_type",
 *   field_ui_base_route = "entity.tripal_entity_type.edit_form"
 * )
 */
class TripalEntity extends ContentEntityBase implements TripalEntityInterface {

  use EntityChangedTrait;

  /**
   * Any errors encountered during the postSave() process.
   *
   * These are saved here to provide context to the TripalEntityForm or any
   * other programatic interface for creating entities.
   *
   * NOTE: We cannot just throw an exception in the postSave() as it mangles
   * the entity. Only inconsequential things should be done in the postSave()
   * and any errors should be handled gracefully.
   *
   * @var array
   *   A list of arrays where each one described an error enountered.
   *   Keys included in sub-array elements are:
   *    - code (string): a developer code for the error.
   *    - exception (bool): indicates if an exception was thrown.
   *    - exception_message (string): the message string of the exception.
   *    - message (string): describes the error encountered. May include tokens.
   *    - message_args (array): tokens with their value for the message.
   */
  protected $post_save_errors = [];

  /**
   * An array of potential token replacement values.
   *
   * @var array
   *   They key is the token name and the value is its value.
   */
  protected $token_values = [];

  /**
   * Keeps track of which TripalStorage Backend each field uses.
   *
   * Note: This will only be populated when the entity has values.
   *
   * @var array
   *   An array keyed by the TripalStorage plugin id (tsid) where the value is
   *   an array of fields that are managed by the given TripalStorage plugin.
   *   In the array of fields, the key is the field machine name and the value
   *   is a boolean indicating if the field is required or not.
   *
   * @see ::registerTripalField()
   */
  public array $tripalstorage_fields = [];

  /**
   * Information on the tripal fields associated with this entity.
   *
   * Note: This can be populated even when this entity does not have any values.
   *
   * @var array
   *   An array keyed by field name where the value provides tripal-specific
   *   information for that field. The following keys are supported:
   *   - term: the ID Space and Accession for this field (e.g. 'schema:name').
   *   - tripalstorage_id: the TripalStorage plugin id which manages this field.
   *   - tripalstorage_settings: the storage plugin settings of this field.
   *   - fully_cached: indicates that all properties for this field are set to
   *     be saved in the Drupal field tables as well as the storage backend.
   *   - main_property: the name of the property which indicates this
   *     field item is empty when it's empty.
   *
   * @see ::registerTripalField()
   */
  public array $tripalfield_info = [];

  /**
   * Save bundles to avoid repeated lookup.
   *
   * @var array
   *   Associative array where the key is bundle ID, value is instance of
   *   Drupal\tripal\Entity\TripalEntityType.
   */
  protected $bundle_cache = [];

  /**
   * {@inheritdoc}
   */
  public static function preCreate(EntityStorageInterface $storage_controller, array &$values) {
    parent::preCreate($storage_controller, $values);
    $values += [
      'uid' => \Drupal::currentUser()->id(),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function create(array $values = []) {
    $entity = parent::create($values);

    // Now that we have the entity created, lets initialize TripalStorage
    // for all of the fields so it can be cached locally.
    $entity->registerAllTripalFields();

    // @debug print "\nWe now have " . count($entity->tripal_storages) . " TripalStorage backends setup in CREATE.\n";
    return $entity;
  }

  /**
   * Allows bundles to be stored in the bundle cache for better performance.
   *
   * @param string $bundle_id
   *   The bundle identifier, e.g. 'organism'.
   * @param Drupal\tripal\Entity\TripalEntityType $bundle
   *   The bundle object to be cached, or NULL can be passed to invalidate
   *   current cached value.
   */
  public function setBundleCache(string $bundle_id, ?TripalEntityType $bundle) {
    if ($bundle) {
      $this->bundle_cache[$bundle_id] = $bundle;
    }
    else {
      unset($this->bundle_cache[$bundle_id]);
    }
  }

  /**
   * Get the bundle object for the current type, and cache it.
   *
   * @return Drupal\tripal\Entity\TripalEntityType
   *   The bundle object
   */
  public function getBundle() {
    $bundle_id = $this->getType();
    $bundle = NULL;
    if (array_key_exists($bundle_id, $this->bundle_cache)) {
      $bundle = $this->bundle_cache[$bundle_id];
    }
    if (!$bundle) {
      $bundle = TripalEntityType::load($bundle_id);
      $this->setBundleCache($bundle_id, $bundle);
    }
    return $bundle;
  }

  /**
   * {@inheritdoc}
   */
  public function getID() {
    $entity_id = $this->id();
    return $entity_id;
  }

  /**
   * {@inheritdoc}
   */
  public function label() {
    $tag_string = \Drupal::config('tripal.settings')->get('tripal_entity_type.allowed_title_tags');
    $tripal_allowed_tags = explode(' ', $tag_string ?? '');

    $title = $this->getTitle();
    $sanitized_value = Xss::filter($title, $tripal_allowed_tags);
    return Markup::create($sanitized_value);
  }

  /**
   * {@inheritdoc}
   */
  public function setTitle($title = NULL) {
    // If no title was passed, construct an entity title.
    if (!$title) {
      $bundle = $this->getBundle();

      // Initialize the Tripal token parser service.
      /** @var \Drupal\tripal\Services\TripalTokenParser $token_parser **/
      $token_parser = \Drupal::service('tripal.token_parser');

      $title_format = $bundle->getTitleFormat();
      $token_values = $this->getBundleEntityTokenValues($title_format, $bundle);
      $title = $token_parser->replaceTokens($title_format, $token_values);
    }

    // HTML token filtering for titles.
    $tag_string = \Drupal::config('tripal.settings')->get('tripal_entity_type.allowed_title_tags') ?? '';
    $allowed_title_tags = explode(' ', $tag_string);
    $title = Xss::filter($title, $allowed_title_tags);

    $this->title = $title;
    return $title;
  }

  /**
   * {@inheritdoc}
   */
  public function getTitle() {
    return $this->title->getString();
  }

  /**
   * Generates a default URL alias for the current entity.
   *
   * @param string $default_alias
   *   Either an empty string if default alias is desired,
   *   or an alias that may optionally contain tokens.
   *
   * @return string
   *   The default entity alias, e.g. "/project/1234"
   */
  public function getDefaultAlias(string $default_alias = '') {
    $bundle = $this->getBundle();

    // Generate an alias using the default format set by admins.
    if (!$default_alias) {
      $default_alias = $bundle->getURLFormat();
    }

    // Initialize the Tripal token parser service and replace tokens.
    /** @var \Drupal\tripal\Services\TripalTokenParser $token_parser **/
    $token_parser = \Drupal::service('tripal.token_parser');
    $token_values = $this->getBundleEntityTokenValues($default_alias, $bundle);
    $default_alias = $token_parser->replaceTokens($default_alias, $token_values);

    // We don't allow HTML tags in the alias.
    $default_alias = strip_tags($default_alias);

    // Ensure there is a leading slash.
    if ($default_alias[0] != '/') {
      $default_alias = '/' . $default_alias;
    }

    // Drupal handles url escaping, but we prefer to replace spaces with dashes.
    $default_alias = str_replace(' ', '-', $default_alias);

    return $default_alias;
  }

  /**
   * Returns the URL alias for the current entity.
   *
   * @return string
   *   The URL alias e.g. "/organism/123"
   */
  public function getAlias() {
    $system_path = '/bio_data/' . $this->getID();
    $langcode = $this->defaultLangcode;
    $existing_alias = \Drupal::service('path_alias.repository')->lookupBySystemPath($system_path, $langcode);
    return $existing_alias;
  }

  /**
   * Sets a URL alias for the current entity if one does not already exist.
   *
   * @param string $path_alias
   *   The alias to use. It can contain tokens that correspond to field values.
   *   Tokens should be be compatible with those returned by
   *   tripal_get_entity_tokens(). If empty, then use the default alias
   *   template. If $path_alias is specified, then any existing alias will
   *   be updated.
   * @param bool $during_save
   *   Indicates if this is being called during the save process or outside
   *   of it. If you are unsure then leave it at the default ;-p.
   *
   * @return string
   *   Returns the path alias that was used with tokens replaced
   */
  public function setAlias(string $path_alias = '', bool $during_save = FALSE): string {

    // Keep track of when a duplicate is found in order to throw an exception
    // at the very end.
    $duplicates = [];

    // Check if an alias already exists for this entity's system path.
    $existing_alias = $this->getAlias();

    // Gets and uses default template, or replaces tokens
    // in the supplied $path_alias.
    $new_alias = $this->getDefaultAlias($path_alias);

    // Check if the specified alias already exists for a different entity.
    // Drupal will check for this for the value from the entity form, but we
    // need to check again for our processed value after token replacement, etc.
    // If it is a duplicate then we remove the alias, and the entity form
    // can complain to the user.
    if (!$existing_alias or ($existing_alias['alias'] != $new_alias)) {
      $entities = \Drupal::entityTypeManager()->getStorage('path_alias')->loadByProperties(['alias' => $new_alias]);
      if ($entities) {

        // Reset the internal path field.
        $path_item = $this->get('path')->first();
        if ($path_item) {
          $path_item->set('alias', '');
          $path_item->set('pid', NULL);
        }

        // Keep track of the duplicates here but DO NOT throw the exception
        // until the end so we can still remove previous alias' if that applies.
        foreach ($entities as $e) {
          $path = $e->getPath();
          $duplicates[$path] = $path;
        }
      }
    }

    // If an alias does not exist, then create one.
    if (!$existing_alias and $new_alias and empty($duplicates)) {
      // The field will create the alias for us so we just need to ensure
      // its set to the new one here.
      $alias_set = FALSE;
      if ($during_save) {
        $path_item = $this->get('path')->first();
        if ($path_item) {
          $path_item->set('alias', $new_alias);
          $alias_set = TRUE;
        }
      }
      // We have to create the path alias ourselves.
      if (!$alias_set) {
        $system_path = '/bio_data/' . $this->getID();
        $new_alias_object = \Drupal::entityTypeManager()->getStorage('path_alias')->create([
          'path' => $system_path,
          'alias' => $new_alias,
        ]);
        if (!is_object($new_alias_object)) {
          throw new \Exception("We were unable to create the alias: '" . $new_alias . "'");
        }
        $new_alias_object->save();
        // And update the internal path field.
        $path_item = $this->get('path')->first();
        if ($path_item) {
          $path_item->set('alias', $new_alias);
          $path_item->set('pid', $new_alias_object->id());
        }
      }
    }
    // If an alias already exists, and is different...
    elseif ($existing_alias and ($existing_alias['alias'] != $new_alias)) {
      $existing_alias_object = \Drupal::entityTypeManager()->getStorage('path_alias')->load($existing_alias['id']);
      if (!is_object($existing_alias_object)) {
        throw new \Exception("Unable to load the existing alias '" . $existing_alias['alias']
            . "' in order to update it.");
      }

      // As long as there were no duplicates, we can update the existing one.
      if (empty($duplicates)) {
        $existing_alias_object->setAlias($new_alias);
        $existing_alias_object->save();
        $path_item = $this->get('path')->first();
        if ($path_item) {
          $path_item->set('alias', $new_alias);
          $path_item->set('pid', $existing_alias['id']);
        }
      }
      // If there are duplcates then we just remove the alias.
      // An exception will be thrown below to inform the user what happened.
      else {
        $existing_alias_object->delete();
        $path_item = $this->get('path')->first();
        if ($path_item) {
          $path_item->set('alias', '');
          $path_item->set('pid', NULL);
        }
      }
    }

    if (!empty($duplicates)) {
      throw new \Exception("We were unable to set the alias '$new_alias' because it already refers to the following: " . implode(', ', $duplicates));
    }

    return $new_alias;
  }

  /**
   * {@inheritdoc}
   */
  public function getType() {
    return $this->bundle();
  }

  /**
   * {@inheritdoc}
   */
  public function getCreatedTime() {
    return $this->get('created')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setCreatedTime($timestamp) {
    $this->set('created', $timestamp);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getChangedTime() {
    return $this->get('changed')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setChangedTime($timestamp) {
    $this->set('changed', $timestamp);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getOwner() {
    return $this->get('uid')->entity;
  }

  /**
   * {@inheritdoc}
   */
  public function getOwnerId() {
    return $this->get('uid')->target_id;
  }

  /**
   * {@inheritdoc}
   */
  public function setOwnerId($uid) {
    $this->set('uid', $uid);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function setOwner(UserInterface $account) {
    $this->set('uid', $account->id());
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function isPublished() {
    return (bool) $this->getEntityKey('status');
  }

  /**
   * {@inheritdoc}
   */
  public function setPublished($published) {
    $this->set('status', $published ? TRUE : FALSE);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getPostSaveErrors() {
    return $this->post_save_errors;
  }

  /**
   * Retrieve the primary key for this content in the backend storage.
   *
   * ASSUMPTION: We assume that the primary key for the backend storage is
   * stored in a `record_id` property present in every required field for a
   * given TripalStorage backend.
   *
   * @param string $tsid
   *   The TripalStorage plugin id for the backend of interest.
   *
   * @return mixed
   *   The primary key for this content in the backend storage, or NULL either
   *   there are no values set for this entity or if it cannot be found.
   */
  public function getBackendRecordId(string $tsid): mixed {
    $backend_record_id = NULL;

    // First only look at fields for this TripalStorage backend.
    if (array_key_exists($tsid, $this->tripalstorage_fields)) {

      // Now filter to those that are required.
      $required_fields = array_keys($this->tripalstorage_fields[$tsid], TRUE, TRUE);

      // Finally we find the first field that has a value and use its
      // record_id as the backend record id.
      foreach ($required_fields as $field_name) {
        if ($this->hasField($field_name) and !$this->get($field_name)->isEmpty()) {
          $first_item = $this->get($field_name)->first();
          $backend_record_id = $first_item->get('record_id')->getValue();
          break;
        }
      }
    }

    return $backend_record_id;
  }

  /**
   * Stores token replacement values for the current entity.
   *
   * @param array $extra_values
   *   Any additional key value pairs to store along with the
   *   values retrieved here, as generated by getBundleEntityTokenValues().
   *
   * @return void
   *   Values are stored in the class variable $this->token_values.
   */
  public function setTokenValues($extra_values = []) {
    $field_values = $this->getFieldValues();
    // Convert to a simple key=>value array.
    $processed_values = $this->processFieldValues($field_values);
    // Merge in any passed values and store.
    // Note: We pass in the original token values to ensure that any values set
    // outside a save() are retained. However, if an updated value for a field
    // exists, it should override previously set tokens, which is why the
    // original tokens are first in the array_merge below.
    $this->token_values = array_merge($this->token_values, $processed_values, $extra_values);
  }

  /**
   * Retrieves the values of the current entity as a nested array.
   *
   * @return array
   *   This is a nested array with the first keys being field names. Within each
   *   array for a given field the keys are delta and the values are an array of
   *   the property names => values for that field delta.
   */
  public function getFieldValues() {
    $values = [];
    $field_defs = $this->getFieldDefinitions();
    foreach ($field_defs as $field_name => $field_def) {
      /** @var \Drupal\Core\Field\FieldItemList $items **/
      $items = $this->get($field_name);
      $values[$field_name] = [];
      /** @var \Drupal\Core\Field\Plugin\Field\FieldType\EntityReferenceItem  $item **/
      foreach ($items as $delta => $item) {
        $values[$field_name][$delta] = [];
        /** @var \Drupal\Core\TypedData\TypedDataInterface $prop **/
        $props = $item->getProperties();
        $main_prop_key = NULL;
        if (method_exists($item, 'mainDisplayPropertyName')) {
          $main_prop_key = $item->mainDisplayPropertyName();
        }
        elseif (method_exists($item, 'mainPropertyName')) {
          $main_prop_key = $item->mainPropertyName();
        }
        if (is_array($props)) {
          foreach ($props as $prop) {
            $prop_name = $prop->getName();
            $prop_value = $prop->getValue();
            $values[$field_name][$delta][$prop_name] = $prop_value;
            // For field-based tokens we replace the token with the value of
            // the main property. We will store this as the 'value' key.
            if ($main_prop_key and ($prop_name == $main_prop_key)) {
              $values[$field_name][$delta]['value'] = $prop_value;
            }
          }
        }
      }
    }
    return $values;
  }

  /**
   * Helper method used for token replacement.
   *
   * Flattens the field values to be suitable for use as values
   * for token replacement.
   *
   * WARNING: Only returns the first value for fields with cardinality > 1.
   *
   * @param array $field_values
   *   Values nested array from $this->getFieldValues()
   *
   * @return array
   *   Associative array of key => value pairs
   */
  protected function processFieldValues(array $field_values): array {
    $processed_values = [];
    foreach ($field_values as $field => $value_array) {
      // Token replacement currently only supports single-value fields,
      // therefore, only add the first value if there are more than one.
      if (count($value_array) >= 1) {
        if (array_key_exists('value', $value_array[0])) {
          $processed_values[$field] = $value_array[0]['value'];
        }
      }
    }
    return $processed_values;
  }

  /**
   * Retrieve values for bundle or entity-specific tokens.
   *
   * These are special tokens like '[TripalEntityType__entity_id]',
   * and for efficiency we only retrieve the value if the token is
   * present in the tokenized string.
   *
   * @param string $tokenized_string
   *   The string containing tokens.
   * @param \Drupal\tripal\Entity\TripalEntityType $bundle
   *   The bundle.
   *
   * @return array
   *   Associative array of all tokens and their values,
   *   ready to use for token replacement.
   */
  protected function getBundleEntityTokenValues(string $tokenized_string, TripalEntityType $bundle) : array {
    // Retrieve the values obtained by $this->setTokenValues()
    $values = $this->token_values;

    // Get the innermost tokens in the string.
    $tokens = [];
    $matches = [];
    if (preg_match_all('/\[([^\[\]]+)\]/', $tokenized_string, $matches)) {
      $tokens = $matches[1];
      foreach ($tokens as $token) {
        $value = NULL;

        // Look for values for bundle or entity related tokens.
        if (($token === 'TripalEntityType__entity_id') or ($token === 'TripalBundle__bundle_id')) {
          $value = $bundle->getID();
        }
        elseif ($token == 'TripalEntityType__label') {
          $value = $bundle->getLabel();
        }
        elseif ($token === 'TripalEntity__entity_id') {
          $value = $this->getID();
        }
        elseif ($token == 'TripalEntityType__term_namespace') {
          $value = $bundle->get('termIdSpace');
        }
        elseif ($token == 'TripalEntityType__term_accession') {
          $value = $bundle->get('termAccession');
        }
        elseif ($token == 'TripalEntityType__term_label') {
          $value = $bundle->getTerm()->getName();
        }
        // We skip over any tokens other than those defined here.
        if (!is_null($value)) {
          $values[$token] = $value;
        }
      }
    }

    return $values;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

    $fields['uid'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Authored by'))
      ->setDescription(t('The username of the content author.'))
      ->setRevisionable(TRUE)
      ->setSetting('target_type', 'user')
      ->setSetting('handler', 'default')
      ->setTranslatable(TRUE)
      ->setDisplayOptions('view', [
        'region' => 'hidden',
        'label' => 'above',
        'type' => 'author',
        'weight' => 0,
      ])
      ->setDisplayOptions('form', [
        'type' => 'entity_reference_autocomplete',
        'weight' => 5,
        'settings' => [
          'match_operator' => 'CONTAINS',
          'size' => '60',
          'autocomplete_type' => 'tags',
          'placeholder' => '',
        ],
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['title'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Title'))
      ->setDescription(t('The title of this specific piece of Tripal Content. This will be automatically updated based on the title format defined by administrators.'))
      ->setSettings([
        'max_length' => 1024,
        'text_processing' => 0,
      ])
      ->setDefaultValue('')
      ->setDisplayOptions('view', [
        'region' => 'hidden',
        'label' => 'above',
        'type' => 'string',
        'weight' => -4,
      ])
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
        'weight' => -4,
      ])
      ->setDisplayConfigurable('view', TRUE);

    $fields['path'] = BaseFieldDefinition::create('path')
      ->setLabel(t('URL alias'))
      ->setDisplayOptions('form', [
        'type' => 'path',
        'weight' => 100,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setComputed(TRUE);

    $fields['status'] = BaseFieldDefinition::create('boolean')
      ->setLabel(t('Publishing status'))
      ->setDescription(t('A boolean indicating whether the Tripal Content is published.'))
      ->setDefaultValue(TRUE);

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Authored on'))
      ->setDescription(t('The date and time that this Tripal Content was created.'))
      ->setTranslatable(TRUE)
      ->setDisplayOptions('view', [
        'region' => 'hidden',
        'label' => 'hidden',
        'type' => 'timestamp',
        'weight' => 0,
      ])
      ->setDisplayOptions('form', [
        'type' => 'datetime_timestamp',
        'weight' => 10,
      ])
      ->setDisplayConfigurable('form', TRUE);

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The date and time that this Tripal Content was last edited.'));

    return $fields;
  }

  /**
   * Registers all the Tripal fields for this entity.
   *
   * @param bool $refresh_cache
   *   Indicates if already registered fields are skipped (default behaviour).
   *   TRUE indicates we should override any existing registration.
   *
   * @return array
   *   Returns a list of TripalStorage instances registered and the fields
   *   each one manages. The TripalStorage plugin id is the key and the
   *   value is an array of field names.
   */
  public function registerAllTripalFields(bool $refresh_cache = FALSE): array {

    // Loop through all fields and register any TripalFields.
    foreach (array_keys($this->getFieldDefinitions()) as $field_name) {
      // @debug print "Registering $field_name.\n";
      $this->registerTripalField($field_name);
    }

    return $this->tripalstorage_fields;
  }

  /**
   * Populates info about this field in the entity for performance reasons.
   *
   * @param string $field_name
   *   The name of the field to be registered.
   * @param bool $refresh_cache
   *   Indicates if this field should be skipped if it is already registered.
   *   TRUE indicates we should override any existing registration.
   *
   * @return bool
   *   TRUE if this field is a TripalField and was registered, FALSE otherwise.
   *
   * @see TripalEntity::getTripalFieldInfo()
   */
  public function registerTripalField(string $field_name, bool $refresh_cache = FALSE): bool {

    // Get some general info about this field to confirm it's a TripalField.
    $field_defn = $this->getFieldDefinition($field_name);
    if (!$field_defn) {
      return FALSE;
    }
    $settings = $field_defn->getSettings();
    $field_storage_defn = $field_defn->getFieldStorageDefinition();

    // Determine some key information to be saved later.
    $is_required = $field_defn->isRequired();

    // Only register TripalFields which use TripalStorage.
    if (!array_key_exists('storage_plugin_id', $settings) or empty($settings['storage_plugin_id'])) {
      return FALSE;
    }

    // TripalStorage Backend ID.
    $tsid = $settings['storage_plugin_id'];

    // Register information about this field from the field definition.
    // Note: Unless it already exists and we're not told to refresh the cache.
    if ((!array_key_exists($field_name, $this->tripalfield_info)) or ($refresh_cache === TRUE)) {

      // @debug print "\tAdded basic details.\n";
      // Next lets save some key information about it.
      $this->tripalfield_info[$field_name]['term'] = $settings['termIdSpace'] . ':' . $settings['termAccession'];
      // -- storage settings.
      $this->tripalfield_info[$field_name]['tripalstorage_id'] = $tsid;
      $this->tripalfield_info[$field_name]['tripalstorage_settings'] = $settings['storage_plugin_settings'];
      // -- field settings.
      $this->tripalfield_info[$field_name]['settings'] = $settings;
      unset($this->tripalfield_info[$field_name]['settings']['storage_plugin_settings']);
      unset($this->tripalfield_info[$field_name]['settings']['storage_plugin_id']);
      $this->tripalfield_info[$field_name]['is_required'] = $is_required;
      $this->tripalfield_info[$field_name]['cardinality'] = $field_storage_defn->getCardinality();
      $this->tripalfield_info[$field_name]['field_type'] = $field_defn->getType();

      // It would be nice to know its class as well.
      $manager = \Drupal::service('plugin.manager.field.field_type');
      $plugin_definition = $manager->getDefinition($field_defn->getType());
      $this->tripalfield_info[$field_name]['field_class'] = $plugin_definition['class'];
      $this->tripalfield_info[$field_name]['field_label'] = $plugin_definition['label'];

      // Now we have the class we can ask for some of the field-specific
      // information like the main property name.
      $this->tripalfield_info[$field_name]['main_property'] = $plugin_definition['class']::mainPropertyName();
      $this->tripalfield_info[$field_name]['main_display_property'] = $plugin_definition['class']::mainDisplayPropertyName();

      // Now lets add information about its property types.
      $property_info = [];
      foreach ($this->tripalfield_info[$field_name]['field_class']::tripalTypes($field_defn) as $property_type) {
        $property_info = [];
        $property_info['id'] = $property_type->getId();
        $property_info['key'] = $property_type->getKey();
        $property_info['cardinality'] = $property_type->getCardinality();
        $property_info['cache_status'] = $property_type->getCacheStatus();
        $property_info['id_space'] = $property_type->getTermIdSpace();
        $property_info['accession'] = $property_type->getTermAccession();
        $property_info = $property_info + $property_type->getStorageSettings();
        $this->tripalfield_info[$field_name]['property_types'][$property_type->getKey()] = $property_info;
      }
    }

    // Register the TripalStorage backend that this field is managed by.
    $this->tripalstorage_fields[$tsid] ??= [];
    $this->tripalstorage_fields[$tsid][$field_name] = $is_required;

    return TRUE;
  }

  /**
   * Get a list of fields managed by a specific backend storage.
   *
   * @param string $tsid
   *   The TripalStorage plugin id (tsid) that controls the backend storage.
   *   of the fields you are interested in.
   * @param array $options
   *   An array of options controlling which fields are returned.
   *   The following are supported:
   *   - is_required (bool): when TRUE only include required fields,
   *     when FALSE only include optional fields.
   *
   * @return array
   *   A list of the fields managed by the specified backend storage.
   */
  public function getTripalStorageFields(string $tsid, array $options = []): array {
    $fields = [];

    // Check if the specified tsid has been registered.
    if (array_key_exists($tsid, $this->tripalstorage_fields)) {

      // Check if the is_required option has been supplied.
      if (array_key_exists('is_required', $options)) {
        // If it has, then we filter tripalstorage_fields for keys with TRUE for
        // required or FALSE for optional.
        if ($options['is_required'] === TRUE) {
          $fields = array_keys($this->tripalstorage_fields[$tsid], TRUE);
        }
        else {
          $fields = array_keys($this->tripalstorage_fields[$tsid], FALSE);
        }
      }
      // If not then return all fields for that storage backend.
      else {
        $fields = array_keys($this->tripalstorage_fields[$tsid]);
      }
    }

    return $fields;
  }

  /**
   * Returns Tripal-specific information about a specific field.
   *
   * This is populated by TripalEntity::registerTripalField() and cached
   * in the class variable $this->tripalfield_info for performance reasons.
   *
   * You can only request information about fields that define a TripalStorage
   * backend (i.e. storage_plugin_id).
   *
   * @param string $field_name
   *   The field that you want information about.
   * @param string $request_key
   *   The information you want. Specifically, the following are supported:
   *   - term (string): the term associated with this field in the format
   *     'termIdSpace:termAccession'.
   *   - tripalstorage_id (string): the TripalStorage plugin id for the
   *     backend that manages this field.
   *   - tripalstorage_settings (array): the settings for the TripalStorage
   *     backend that manages this field.
   *   - settings (array): the field settings defined in Drupal for this field.
   *   - is_required (bool): indicates whether this field is required or not.
   *   - cardinality (int): the cardinality of this field.
   *   - field_type (string): the Drupal field type for this field.
   *   - field_class (string): the class name for this field.
   *   - field_label (string): the human readable label for this field.
   *   - property_types (array): an array of the property types for this field,
   *     keyed by the property key. Each element in the array is an array with
   *     the following keys:
   *     - id (string): the id of the property type (e.g. 'varchar').
   *     - key (string): the property key set in the field class (e.g. 'value').
   *     - cardinality (int): the cardinality of this property.
   *     - cache_status (bool): the cache status of this property, where TRUE
   *       indicates this property is cached in the Drupal field tables and
   *       FALSE indicates it is not.
   *     - storage_settings (array): the storage settings used by the backend
   *       storage for this property.
   *     - id_space (string): the term id space for this property type.
   *     - accession (string): the term accession for this property type.
   *   - main_property (string): the key name for the main property in this
   *     field. This is set by TripalFieldItem::mainPropertyName() and is the
   *     property used to test if the field is empty.
   *   - main_display_property (string): the key name for the property used to
   *     generate the field value for token replacement in title and URL.
   *
   * @return mixed
   *   The information indicated by $request_key for the field indicated. See
   *   The $request_key param for the return value to expect for a specific
   *   request key.
   *
   * @throws \Exception
   *   An exception is thrown in the following cases:
   *   - the field is not cached after populating it. This happens if the field
   *     is not a valid Tripal Field.
   *   - the request key is not supported. See supported keys above.
   *
   * @group tripal-storage
   */
  public function getTripalFieldInfo(string $field_name, string $request_key): mixed {
    // Ensure this field is registered.
    $this->registerTripalField($field_name);

    if (!array_key_exists($field_name, $this->tripalfield_info)) {
      throw new \Exception("You requested information for a field (i.e. '$field_name') that is either not attached to this entity or not a valid TripalField.");
    }

    if (!array_key_exists($request_key, $this->tripalfield_info[$field_name])) {
      throw new \Exception("The Request key '$request_key' is not supported by TripalEntity::getTripalFieldInfo(). This error was encountered when information was requested for '$field_name' field.");
    }

    // Now we can use that TripalField information cache to retrieve the
    // requested information.
    return $this->tripalfield_info[$field_name][$request_key];
  }

  /**
   * Returns TripalStorage-specific information about a specific field.
   *
   * @param string $field_name
   *   The field that you want information about.
   * @param string $request_key
   *   The information you want. This will depend on the TripalStorage backend
   *   used by this field, but some examples include:
   *   - base_table (string): the table to store this field in the backend.
   *   - base_column (string): the column in the base table.
   *
   * @return mixed
   *   The information indicated by $request_key for the field indicated.
   */
  public function getTripalFieldStorageInfo(string $field_name, string $request_key): mixed {

    // Ensure this field is registered.
    $this->registerTripalField($field_name);

    if (!array_key_exists($field_name, $this->tripalfield_info)) {
      throw new \Exception("You requested field storage information for a field (i.e. '$field_name') that is either not attached to this entity or not a valid TripalField.");
    }

    if (!array_key_exists($request_key, $this->tripalfield_info[$field_name]['tripalstorage_settings'])) {
      throw new \Exception("The Request key '$request_key' is not supported by TripalEntity::getTripalFieldStorageInfo(). This error was encountered when information was requested for '$field_name' field.");
    }

    // Now we can use that TripalField information cache to retrieve the
    // requested information.
    return $this->tripalfield_info[$field_name]['tripalstorage_settings'][$request_key];
  }

  /**
   * Returns a list of property keys for a specific field.
   *
   * @param string $field_name
   *   The name of the field we want to get the property keys for.
   *
   * @return array
   *   A simple list of the property keys for this field.
   */
  public function getTripalFieldPropertyKeys(string $field_name): array {
    // Ensure this field is registered.
    $this->registerTripalField($field_name);

    if (!array_key_exists($field_name, $this->tripalfield_info)) {
      throw new \Exception("You requested field property keys for a field (i.e. '$field_name') that is either not attached to this entity or not a valid TripalField.");
    }

    return array_keys($this->tripalfield_info[$field_name]['property_types']);
  }

  /**
   * Returns Tripal Property Type-specific information about a specific field.
   *
   * @param string $field_name
   *   The field that you want information about.
   * @param string $property_key
   *   The property key for which you want information.
   * @param string $request_key
   *   The information you want. This will depend on the TripalStorage backend
   *   used by this field, but some examples include:
   *   - cache_status (bool): whether or not this property is cached in the
   *     Drupal field tables. TRUE indicates this property is cached and
   *     FALSE indicates it is not.
   *   - action (string): the action to use for storage backend.
   *   - path (string): the path describing the column to save this property
   *   to in the backend.
   *   - table_alias_mapping (array): a mapping between the alias used in the
   *   path and the real table name in the backend.
   *
   * @return mixed
   *   The information indicated by $request_key for the field indicated.
   *   If the request_key is not defined then NULL is returned.
   */
  public function getTripalFieldPropertyInfo(string $field_name, string $property_key, string $request_key): mixed {

    // Ensure this field is registered.
    $this->registerTripalField($field_name);

    if (!array_key_exists($field_name, $this->tripalfield_info)) {
      throw new \Exception("You requested field property information for a field (i.e. '$field_name') that is either not attached to this entity or not a valid TripalField.");
    }

    if (!array_key_exists($property_key, $this->tripalfield_info[$field_name]['property_types'])) {
      throw new \Exception("You requested field property information for a property (i.e. '$property_key') that is not part of the '$field_name' field.");
    }

    if (!array_key_exists($request_key, $this->tripalfield_info[$field_name]['property_types'][$property_key])) {
      return NULL;
    }

    // Now we can use that TripalField information cache to retrieve the
    // requested information.
    return $this->tripalfield_info[$field_name]['property_types'][$property_key][$request_key];
  }

  /**
   * Returns the path for a specific property of a field.
   *
   * @param string $field_name
   *   The field containing the property for which you want the path.
   * @param string $property_key
   *   The property key for which you want the path.
   *
   * @return ?string
   *   The path describing the column to save this property to in the backend.
   *   If the path is not defined for this property then NULL is returned.
   */
  public function getTripalFieldPropertyPath(string $field_name, string $property_key): ?string {
    // Ensure this field is registered.
    $this->registerTripalField($field_name);

    if (!array_key_exists($field_name, $this->tripalfield_info)) {
      throw new \Exception("You requested a property path for a field (i.e. '$field_name') that is either not attached to this entity or not a valid TripalField.");
    }

    if (!array_key_exists($property_key, $this->tripalfield_info[$field_name]['property_types'])) {
      throw new \Exception("You requested a property path for a property (i.e. '$property_key') that is not part of the '$field_name' field.");
    }

    $path = $this->getTripalFieldPropertyInfo($field_name, $property_key, 'path');
    $mapping = $this->getTripalFieldPropertyInfo($field_name, $property_key, 'table_alias_mapping');
    if (is_array($mapping)) {
      foreach ($mapping as $alias => $real_table) {
        $path = str_replace($alias . '.', $real_table . '.', $path);
      }
    }

    return $path;
  }

  /**
   * Gets an array of field item lists including only TripalField instances.
   *
   * This is a filtered version of ContentEntityBase::getFields(). We use it in
   * cases where we need to only act on TripalFields using backend storage.
   * This saves us from checking the interface and the tsid every single time.
   *
   * NOTE: This requires TripalStorage to have been setup for the current
   * entity instance. If it has not yet been then, this function will do that
   * first.
   *
   * @return \Drupal\Core\Field\FieldItemListInterface[]
   *   An array of field item lists for only TripalFields, keyed by field name.
   */
  public function getTripalFieldItems() {
    $tripalfield_items = [];

    // Now we can use that TripalField information cache to get a list of
    // fields that have implemented the TripalFieldItemInterface and indicated
    // a Tripal Storage backend.
    foreach (array_keys($this->tripalfield_info) as $field_name) {
      $tripalfield_items[$field_name] = $this->get($field_name);
    }

    return $tripalfield_items;
  }

  /**
   * Returns an associative array of StoragePropertyValues for the entity.
   *
   * @param TripalEntity $entity
   *   The entity to retrieve a values array for.
   * @param bool $ignore_cached_fields
   *   Whether or not to ignore the cache. Specifically, if TRUE then values
   *   will be retrieved fresh from the storage backend even if they were
   *   already cached. If FALSE then the cached value will be used.
   *
   * @return array
   *   An array of values as described used when managing TripalStorage
   *   backends. Specifically, the array is keyed in the following levels:
   *   - 1st: Tripal Stroage Plugin ID
   *   - 2nd: Field name
   *   - 3rd: Delta value of the field item.
   *   - 4th: the property key.
   *   - 5th: One of the following keys:
   *     - 'value': the property value object.
   *     - 'operation': the operation to use when matching this value.
   */
  public static function getValuesArray($entity, $ignore_cached_fields = FALSE) {
    $values = [];
    $tripal_storages = [];

    // Specifically, for each field...
    foreach ($entity->getTripalFieldItems() as $field_name => $items) {
      foreach ($items as $item) {

        $storage = self::getFieldItemBackendStorage($field_name, $item);
        if ($storage === FALSE) {
          continue;
        }
        [$delta, $tsid] = $storage;

        // Create instance of the storage plugin so we can add the properties
        // to it as we go.
        if (!array_key_exists($tsid, $tripal_storages)) {
          $tripal_storage = \Drupal::service("tripal.storage")->getInstance(['plugin_id' => $tsid]);
          $tripal_storages[$tsid] = $tripal_storage;
        }

        // Get the property values for this field item
        // and the property type objects.
        $prop_values = $item->syncTripalStoragePropertyValues();
        $prop_types = $item->getTripalStoragePropertyTypes();

        // Ensure that only the properties that should be are cleared.
        // Note: fully_cached will only be true for this field if all
        // properties for this field are cached in the drupal field tables.
        $fully_cached = $tripal_storages[$tsid]->markPropertiesForCaching(
          $field_name,
          $prop_types
        );
        // $this->tripalfield_info[$field_name]['fully_cached'] = $fully_cached;
        // Only setup TripalStorage for this field if it is not cached
        // or if we are not ignoring cached fields right now.
        if (!$fully_cached or ($ignore_cached_fields == FALSE)) {

          // Add the field definition to the storage for this field.
          $tripal_storages[$tsid]->addFieldDefinition($field_name, $item->getFieldDefinition());

          // Clears the values from the entity (does not clear them from the
          // property).
          $item->clearFieldValuesForTripalStorage();

          // Add the property types to the storage plugin.
          $tripal_storages[$tsid]->addTypes($field_name, $prop_types);

          // Prepare the property values for the storage plugin.
          foreach ($prop_values as $prop_key => $prop_value) {
            $values[$tsid][$field_name][$delta][$prop_key] ?? [];
            $values[$tsid][$field_name][$delta][$prop_key]['value'] = $prop_value;
          }
        }
      }
    }
    return [$values, $tripal_storages];
  }

  /**
   * Updates the fields in the entity with the values from Tripal Storage.
   *
   * This method is expected to be called as part of the TripalStorage backend
   * load workflow. Specifically, the entity is prepared using getValuesArray(),
   * the values are loaded for each backend using TripalStorage::loadValues()
   * and then this method processes those values in order to update the fields
   * on the original entity.
   *
   * @param TripalEntity $entity
   *   The entity that we want to update.
   * @param array $values
   *   Values returned from TripalStorage mapping to fields of this entity.
   * @param array $tripal_storages
   *   Array of TripalStorage objects.
   * @param bool $do_save
   *   TRUE indicates this is being called within the save workflow and
   *   FALSE when it is being called in the load workflow.
   *
   * @return array
   *   This method returns context that may be used in the calling method.
   *   Current context:
   *   - empty_items: a nested array of [field_name][delta] = delta for each
   *     field item which is determined to be empty via isEmptyFieldItem().
   *
   * @see TripalEntityHooks::tripalEntityStorageLoad()
   */
  public static function saveValuesArray(TripalEntity &$entity, array &$values, array &$tripal_storages, bool $do_save = FALSE) {
    $context = [
      // @todo remove this once we handle this using the field::isEmpty().
      'empty_items' => [],
    ];

    // Update the entity values with the values returned by loadValues().
    foreach ($entity->getTripalFieldItems() as $field_name => $items) {

      // @todo remove this once we handle this using the field::isEmpty().
      $context['empty_items'][$field_name] ??= [];

      foreach ($items as $k => $item) {

        // This is a simple key => value array which will be given to the
        // field to update it's internal StoragePropertyValues.
        $new_values = [];

        $storage = self::getFieldItemBackendStorage($field_name, $item);
        if ($storage === FALSE) {
          continue;
        }
        [$delta, $tsid] = $storage;

        // Create a new properties array for this field item.
        $prop_values = [];
        $store_values = [];
        foreach ($values[$tsid][$field_name][$delta] as $property_key => $info) {

          // Get the new value for this property.
          $new_property_value = $info['value']->getValue();
          $new_values[$property_key] = $new_property_value;

          // Store the values of any properties with a "store" action.
          // There will usually only be one, exceptions are dbxref,
          // relationship.
          // @todo remove this once we handle this using the field::isEmpty().
          $prop_type = $tripal_storages[$tsid]->getPropertyType($field_name, $property_key);
          if (self::isStorePropType($prop_type)) {
            $store_values[$property_key] = $new_property_value;
          }
          $prop_values[] = $info['value'];

          // If we are doing a save, then we do not want to save the new value
          // of any properties which are not meant to be stored in the Drupal
          // field tables.
          $store_in_drupal = $tripal_storages[$tsid]->isDrupalStoreByFieldNameKey($field_name, $property_key);
          if ($do_save && $store_in_drupal === FALSE) {
            $new_values[$property_key] = $info['value']->getDefaultValue();
          }
        }

        // Now we can update the internal property values for this field.
        $item->updateTripalStoragePropertyValues($new_values);
        $item->syncFieldValuesWithTripalStorage();

        // Keep track of empty field items in case the calling method needs
        // this information.
        // @todo remove this once we handle this using the field::isEmpty().
        if (self::isEmptyFieldItem($field_name, $items, $prop_values, $store_values) === TRUE) {
          $context['empty_items'][$field_name][$delta] = $delta;
        }

        // Set the item back to the list.
        $items->set($k, $item);
      }
    }

    return $context;
  }

  /**
   * Retrieve Tripal Backend storage for a TripalField item.
   *
   * @param string $field_name
   *   The name of the field this item is for.
   * @param Drupal\Core\Field\FieldItemInterface $item
   *   The item whose backend storage we want to retrieve.
   *
   * @return array|bool
   *   FALSE if this is not a TripalFieldItem or if it doesn't indicate its
   *   TripalStorage plugin. Otherwise, an associative array describing the
   *   backend storage for this item. Specifically,
   *   - delta: the delta of this item in the fielditemlist it came from.
   *   - tsid: the tripalstorage id for its storage backend.
   *   - storage: an instance of this items tripalstorage backend.
   */
  public static function getFieldItemBackendStorage(string $field_name, FieldItemInterface $item): bool|array {

    // This must be a TripalField item.
    if (!$item instanceof TripalFieldItemInterface) {
      return FALSE;
    }

    $delta = $item->getName();
    $tsid = $item->tripalStorageId();

    // If the Tripal Storage Backend is not set on a Tripal-based field,
    // we log an error and will not support the field. If developers want
    // to use Drupal storage for a Tripal-based field then they need to
    // indicate that by using our Drupal SQL Storage option OR by not
    // creating a Tripal-based field at all depending on their needs.
    if (empty($tsid)) {
      throw new \Exception("The Tripal-based field '$field_name' on this content type must indicate a TripalStorage backend and currently does not.");
    }

    return [
      $delta,
      $tsid,
    ];
  }

  /**
   * Helper function: check if a field item is empty based on property values.
   *
   * @param string $field_name
   *   The name of the field these properties are associated with and whom
   *   we want to determine its emptiness.
   * @param Drupal\Core\Field\FieldItemList $items
   *   The current items for this field that match with the values.
   * @param array $prop_values
   *   An array of property value objects for the current field item.
   * @param array $store_values
   *   A mapping of property key => value for property types with store action.
   *
   * @return bool
   *   TRUE if this field item is considered empty and FALSE otherwise.
   */
  public static function isEmptyFieldItem(string $field_name, FieldItemList $items, array $prop_values, array $store_values) {

    // Does this field item have only empty values?
    // If yes, it should be removed.
    if (self::allNull($prop_values)) {
      return TRUE;
    }

    // If there is a zero value in $store_values, this means that
    // we chose "- Select -" in a widget, or removed the row with the
    // "Remove" button.
    // For properties or other single-hop fields we check the main property
    // value for a NULL or empty string. Note that in this case, other
    // $store_values may not be empty, e.g. type_id for a property.
    // Chado storage has already done its work, so now remove this
    // delta so that Drupal doesn't make a blank field table entry.
    $main_property_name = self::getMainPropertyName($field_name, $items);
    foreach ($store_values as $key => $value) {
      if ($value === 0) {
        return TRUE;
      }
      if ($key == $main_property_name && ($value === NULL || $value === '')) {
        return TRUE;
      }
    }

    return FALSE;
  }

  /**
   * Helper function: is this a property type and if yes, is its action store?
   *
   * @param ?object $prop_type
   *   What we think should be a property type. We do need to check that is is.
   *
   * @return bool
   *   TRUE if this is a property type and it's action is STORE
   *   and FALSE otherwise.
   */
  public static function isStorePropType(?object $prop_type): bool {

    // First get the action for this prop type.
    $action = '';
    if ($prop_type) {
      $action = $prop_type->getStorageSettings()['action'] ?? '';
    }

    // Now indicate if this is a store property type based on that action.
    return ($action == 'store') ? TRUE : FALSE;
  }

  /**
   * Helper function: Confirm array contains all null elements.
   *
   * @param array $array_to_check
   *   The array to check for null values. It is expected to be a flat array.
   *
   * @return bool
   *   True if all elements are null; False if even one element is not null.
   */
  public static function allNull(array $array_to_check) : bool {
    foreach ($array_to_check as $value) {
      if (isset($value)) {
        return FALSE;
      }
    }
    return TRUE;
  }

  /**
   * Returns the name of the main property for a field.
   *
   * The main property name defaults to 'value', but a field can define
   * a function mainPropertyName() to indicate a different name.
   *
   * @param string $field_name
   *   The machine name of the field.
   * @param Drupal\Core\Field\FieldItemList $items
   *   The current items for this field that match with the values.
   *
   * @return string
   *   The main property name for this field.
   */
  public static function getMainPropertyName(string $field_name, FieldItemList $items): string {
    $main_property_name = 'value';

    foreach ($items as $item) {
      if (method_exists($item, 'mainPropertyName')) {
        $main_property_name = $item->mainPropertyName();
      }
      // We only need to examine the first item.
      break;
    }
    return $main_property_name;
  }

  /**
   * {@inheritdoc}
   */
  public function preSave(EntityStorageInterface $storage): void {
    parent::preSave($storage);

    // Create a values array appropriate for `loadValues()`
    [$values, $tripal_storages] = TripalEntity::getValuesArray($this);

    // Perform the Insert or Update of the submitted values to the
    // underlying data store.
    foreach ($values as $tsid => $tsid_values) {

      // Do an insert.
      if ($this->isDefaultRevision() and $this->isNewRevision()) {
        try {
          $tripal_storages[$tsid]->insertValues($tsid_values);
        }
        catch (\Exception $e) {
          \Drupal::logger('tripal')->error($e->getMessage());
          \Drupal::messenger()->addError('Cannot insert this entity. See the recent ' .
              'logs for more details or contact the site administrator if you ' .
              'cannot view the logs.');
          // We cannot safely continue after such error.
          return;
        }
        $values[$tsid] = $tsid_values;
      }

      // Do an Update.
      else {
        try {
          $tripal_storages[$tsid]->updateValues($tsid_values);
        }
        catch (\Exception $e) {
          \Drupal::logger('tripal')->error($e->getMessage());
          \Drupal::messenger()->addError('Cannot update this entity. See the recent ' .
              'logs for more details or contact the site administrator if you cannot ' .
              'view the logs.');
          // We cannot safely continue after such error.
          return;
        }
      }

      // Right now the assumption that only key values will be saved is baked
      // into ChadoStorage insert/update. That means, the non-key properties
      // do not have a value after saving because ChadoStorage didn't bother
      // to set them... if it did, then the following loadValues would not be
      // needed since the values would already be set.
      // @todo look into fixing insert/update to return all values.
      // NOTE: We use FALSE here so the values are loaded from the database.
      $tripal_storages[$tsid]->loadValues($tsid_values, FALSE);
    }

    // Set the property values that should be saved in Drupal, everything
    // else will stay in the underlying data store (e.g. Chado).
    $context = self::saveValuesArray($this, $values, $tripal_storages, TRUE);
    $delta_remove = $context['empty_items'];

    // Now remove any values that shouldn't be there.
    foreach ($delta_remove as $field_name => $deltas) {
      foreach (array_reverse($deltas) as $delta) {
        try {
          $this->get($field_name)->removeItem($delta);
        }
        catch (\Exception $e) {
          \Drupal::logger('tripal')->error($e->getMessage());
          \Drupal::messenger()->addError('Cannot insert this entity. See the recent ' .
              'logs for more details or contact the site administrator if you ' .
              'cannot view the logs.');
        }
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function postSave(EntityStorageInterface $storage, $update = TRUE) {

    // Set the tokens for title/URL replacement now so that they include all
    // of the field values (i.e. set it before Tripal/Chado storage clears any).
    $this->setTokenValues();

    // We need to generate the title here since it requires tokens to already
    // have been populated/saved in the entity. Since save has already happened,
    // we need to directly write to the base table to update the title.
    $title = $this->setTitle();
    $base_table = $storage->getBaseTable();
    $entity_id = $this->id();
    if ($base_table and $entity_id) {
      try {
        \Drupal::service('database')->update($base_table)
          ->fields(['title' => $title])
          ->condition('id', $entity_id)
          ->execute();
      }
      catch (\Exception $e) {
        // Throwing an exception in postSave() mangles the entity!
        // Warn the curator that the title was not set so they can fix it.
        // Drupal does not require unique titles so it is ok to leave
        // things in this state.
        $this->post_save_errors[] = [
          'code' => 'TITLE-DB-SAVE',
          'exception' => TRUE,
          'exception_message' => $e->getMessage(),
          'message' => "We were unable to update the title ':title' directly.  Once the root cause is fixed, the title can be created by updating this :bundle.",
          'message_args' => [':title' => $title, ':bundle' => $this->getBundle()->label()],
        ];
      }
      // If title is blank or is only HTML tokens, show a warning.
      if (!trim(strip_tags($title))) {
        $this->post_save_errors[] = [
          'code' => 'TITLE-DB-SAVE',
          'exception' => FALSE,
          'message' => "The entity title is currently blank. You should either update this source record, or modify the title format tokens for :bundle.",
          'message_args' => [':bundle' => $this->getBundle()->label()],
        ];
      }
    }

    // We also want to set the URL alias here for the same reason we set the
    // title at this point. This setter does not need a save of the entity
    // afterwards so calling it should be sufficient. If an empty string is
    // passed to setAlias() then an alias is generated based on the url_format.
    $path_alias = '';
    $path_values = $this->get('path')->getValue();
    if (array_key_exists(0, $path_values) && array_key_exists('alias', $path_values[0])) {
      $path_alias = (string) $path_values[0]['alias'];
    }
    try {
      $this->setAlias($path_alias, TRUE);
    }
    catch (\Exception $e) {
      // Throwing an exception in postSave() mangles the entity!
      // Warn the curator that the URL alias was not set so they can fix it.
      // This entity will just not have an alias.
      $this->post_save_errors[] = [
        'code' => 'URL-ALIAS-SAVE',
        'exception' => TRUE,
        'exception_message' => $e->getMessage(),
        'message' => $e->getMessage(),
        'message_args' => [],
      ];
    }

    // Now we've done our last minute processing let the parent do it's thing.
    // This includes postSaving all the fields which is where the path field
    // will create the alias if one is provided.
    parent::postSave($storage, $update);
  }

  /**
   * {@inheritdoc}
   */
  public function validate() {
    // Let the parent class do its validations and return the violations list.
    $violations = parent::validate();

    // Create a values array appropriate for `loadValues()`
    [$values, $tripal_storages] = TripalEntity::getValuesArray($this);

    // Iterate through the different Tripal Storage objects and run the
    // validateValues() function for the values that belong to it.
    foreach ($values as $tsid => $tsid_values) {
      $problems = $tripal_storages[$tsid]->validateValues($tsid_values);
      foreach ($problems as $violation) {
        $violations->add($violation);
      }
    }

    return $violations;
  }

  /**
   * Performs a removal of the entity from Drupal.
   *
   * This function copies the code from the parent::delete() function.  It
   * does not remove the record from the storage backend. The
   * postDelete() function will be triggered.
   */
  public function unpublish() {
    parent::delete();
  }

  /**
   * Performs a total remove of the record from Drupal and the DB backend.
   *
   * This function copies the code from the parent::delete() function but
   * then performs extra steps to delete the record in the database backend.
   * The postDelete() function will also be triggered because it uses the
   * parent::delete() function to delete the entity from Drupal.
   */
  public function delete() {
    parent::delete();

    // Create a values array appropriate for `deleteValues()`
    [$values, $tripal_storages] = TripalEntity::getValuesArray($this);

    // Call the deleteValues() function for each storage type.
    $delete_success = FALSE;
    foreach ($values as $tsid => $tsid_values) {
      try {
        $delete_success = $tripal_storages[$tsid]->deleteValues($tsid_values);
        if ($delete_success) {
          $values[$tsid] = $tsid_values;
        }
      }
      catch (\Exception $e) {
        \Drupal::logger('tripal')->notice($e->getMessage());
        \Drupal::messenger()->addError('Cannot delete the entity. See the recent ' .
            'logs for more details or contact the site administrator if you cannot ' .
            'view the logs.');
      }
    }
  }

}
