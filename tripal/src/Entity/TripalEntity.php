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
   * Save bundles to avoid repeated lookup.
   *
   * @var array
   *   Associative array where the key is bundle ID, value is instance of
   *   Drupal\tripal\Entity\TripalEntityType.
   */
  protected $bundle_cache = [];

  /**
   * Keeps track of information about Tripal fields + their property types.
   *
   * This is set during getValuesArray() and reused in saveValuesArray() as well
   * as other places.
   *
   * @var array
   *   This is an associative array keyed by the field_name. For each field,
   *   the value is an array with the following supported keys:
   *   - store_properties: the keys of property types with action = store.
   */
  protected array $tripalfield_info = [];

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
        $path_item = $this->path->first();
        $path_item->set('alias', '');
        $path_item->set('pid', NULL);

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
      if ($during_save) {
        $path_item = $this->path->first();
        $path_item->set('alias', $new_alias);
      }
      // We have to create the path alias ourselves.
      else {
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
        $path_item = $this->path->first();
        $path_item->set('alias', $new_alias);
        $path_item->set('pid', $new_alias_object->id());
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
        $path_item = $this->path->first();
        $path_item->set('alias', $new_alias);
        $path_item->set('pid', $existing_alias['id']);
      }
      // If there are duplcates then we just remove the alias.
      // An exception will be thrown below to inform the user what happened.
      else {
        $existing_alias_object->delete();
        $path_item = $this->path->first();
        $path_item->set('alias', '');
        $path_item->set('pid', NULL);
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
        if (method_exists($item, 'mainPropertyName')) {
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
   * Returns an associative array of property type values for the entity.
   *
   * The array is keyed in the following levels:
   * - 1st: Tripal Stroage Plugin ID
   * - 2nd: Field name
   * - 3rd: Delta value of the field item.
   * - 4th: the property key.
   * - 5th: One of the following keys:
   *   - 'value': the property value object.
   *   - 'operation': the operation to use when matching this value.
   *
   * This function also returns an array of TripalStorage objects.
   *
   * @param TripalEntity $entity
   *   The entity to retrieve a values array for.
   * @param bool $ignore_cached_fields
   *   Whether or not to ignore the cache. Specifically, if TRUE then values
   *   will be retrieved fresh from the storage backend even if they were
   *   already cached. If FALSE then the cached value will be used.
   *
   * @return array
   *   The returned array has two elements: an array of values as described
   *   above, and an array of TripalStorage objects,
   */
  public static function getValuesArray($entity, $ignore_cached_fields = FALSE) {
    $values = [];
    $tripal_storages = [];
    $fields = $entity->getFields();

    // Specifically, for each field...
    foreach ($fields as $field_name => $items) {
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

        // Get the empty property values for this field item and the
        // property type objects.
        $prop_values = $item->tripalValuesTemplate($item->getFieldDefinition());
        $prop_types = $entity->getFieldPropertyTypes($field_name);

        // Ensure that only the properties that should be are cleared.
        // Note: is_cached will only be true for this field if all properties
        // for this field are cached in the drupal field tables.
        $is_cached = $tripal_storage->markPropertiesForCaching($field_name, $prop_types);

        // Only setup TripalStorage for this field if it is not cached
        // or if we are not ignoring cached fields right now.
        if (!$is_cached or ($ignore_cached_fields == FALSE)) {

          // Add the field definition to the storage for this field.
          $tripal_storages[$tsid]->addFieldDefinition($field_name, $item->getFieldDefinition());

          // Sets the values from the entity on both the property and in entity.
          // Despite the function name, no values are saved to the database.
          $item->tripalSave($item, $field_name, $prop_types, $prop_values, $entity);

          // Clears the values from the entity (does not clear them from the
          // property).
          $item->tripalClear($item, $field_name, $prop_types, $prop_values, $entity);

          // Add the property types to the storage plugin.
          $tripal_storages[$tsid]->addTypes($field_name, $prop_types);

          // Prepare the property values for the storage plugin.
          // Note: We are assuming the key for the value is the
          // same as the key for the type here... This is a temporary assumption
          // as soon the values array will not contain types ;-)
          foreach ($prop_types as $prop_type) {
            $key = $prop_type->getKey();
            $values[$tsid][$field_name][$delta][$key] = [];
          }
          foreach ($prop_values as $prop_value) {
            $key = $prop_value->getKey();
            $values[$tsid][$field_name][$delta][$key]['value'] = $prop_value;
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
   *   when FALSE if is being called in the load workflow.
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
      'empty_items' => [],
    ];

    // Update the entity values with the values returned by loadValues().
    $field_items = $entity->getFields();
    foreach ($field_items as $field_name => $items) {
      $context['empty_items'][$field_name] ??= [];
      foreach ($items as $k => $item) {

        $storage = self::getFieldItemBackendStorage($field_name, $item);
        if ($storage === FALSE) {
          continue;
        }
        [$delta, $tsid] = $storage;

        // Create a new properties array for this field item.
        $prop_values = [];
        $prop_types = [];
        $store_values = [];
        foreach ($values[$tsid][$field_name][$delta] as $key => $info) {

          // Get the specific prop type and it's corresponding value.
          $prop_type = $tripal_storages[$tsid]->getPropertyType($field_name, $key);
          $prop_value = $info['value'];

          // Store the values of any properties with a "store" action.
          // There will usually only be one, exceptions are dbxref,
          // relationship.
          if (self::isStorePropType($prop_type)) {
            $store_values[$key] = $prop_value->getValue();
          }

          // We do some extra work here when saving
          // related to conditionally saving field values.
          if ($do_save && $tripal_storages[$tsid]->isDrupalStoreByFieldNameKey($field_name, $key)) {
            $prop_values[] = $prop_value;
            $prop_types[] = $prop_type;
          }
          // When loading we add all property types/fields.
          elseif ($do_save === FALSE) {
            $prop_values[] = $prop_value;
            $prop_types[] = $prop_type;
          }
        }

        // Now set the entity values for this field.
        if (count($prop_values) > 0) {
          $item->tripalLoad($item, $field_name, $prop_types, $prop_values, $entity);

          // Keep track of empty field items in case the calling method needs
          // this information.
          if (self::isEmptyFieldItem($field_name, $items, $prop_values, $store_values) === TRUE) {
            $context['empty_items'][$field_name][$delta] = $delta;
          }
        }

        // Set the item back to the list.
        $items->set($k, $item);
      }
    }

    return $context;
  }

  /**
   * Retrieves the TripalStorage property types for a given field.
   *
   * @param string $field_name
   *   The name of the field we want property types for.
   * @param bool $use_cache
   *   Indicates if we should use the cache when it's availabe. When set to
   *   FALSE, the property types will be updated even when cached.
   *
   * @return array
   *   An array of property type objects for this field.
   */
  protected function getFieldPropertyTypes(string $field_name, bool $use_cache = TRUE): array {
    $field_defn = $this->getFieldDefinition($field_name);

    // Check if this fields info is cached at all... if not then add it.
    if (!array_key_exists($field_name, $this->tripalfield_info)) {
      $field_class = $field_defn->getItemDefinition()->getClass();

      $this->tripalfield_info[$field_name] = [
        'class' => $field_class,
      ];
    }

    // Check if the property types have been cached and look them up if not.
    if (!array_key_exists('property_types', $this->tripalfield_info) or !$use_cache) {
      $field_class ??= $this->tripalfield_info[$field_name]['class'];
      $field_defn ??= $this->getFieldDefinition($field_name);
      $prop_types = $field_class::tripalTypes($field_defn);

      $this->tripalfield_info[$field_name]['property_types'] = $prop_types;

      // We also want to take advantage of the moment these are cached to index
      // some information about them for faster use later.
      foreach ($prop_types as $key => $prop_type) {

        // Index based on their actions.
        $action = $prop_type->getStorageSettings()['action'] ?? 'MISSING_ACTION';
        $this->tripalfield_info[$field_name][$action . '-properties'][$key] = $key;
      }
    }

    return $this->tripalfield_info[$field_name]['property_types'];
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
   *   - tsid: the tripalstorage id for it's storage backend.
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
      \Drupal::logger('tripal')->error('The Tripal-based field :field on
            this content type must indicate a TripalStorage backend and currently does not.',
        [':field' => $field_name]
      );
      return FALSE;
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
   *   The array to check for null values. It is expected to be a flat array
   *   where the value is either a simple value OR a PropertyValue object.
   *
   * @return bool
   *   TRUE if all elements are null; FALSE if even one element is not null.
   */
  public static function allNull(array $array_to_check) : bool {

    foreach ($array_to_check as $value) {

      // If we were given an object then get the value from it first.
      if (is_object($value)) {
        $value = $value->getValue();
      }

      // Check if the value is defined and not null.
      if (isset($value)) {
        return FALSE;
      }
    }

    // If we get here then all values are null.
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

    // Set a string to be used in messages if this process fails.
    $mode_string = ($this->isDefaultRevision() and $this->isNewRevision()) ? 'create' : 'update';

    try {

      // Create a values array appropriate for `loadValues()`
      [$values, $tripal_storages] = TripalEntity::getValuesArray($this);

      // Perform the Insert or Update of the submitted values to the
      // underlying data store.
      foreach ($values as $tsid => $tsid_values) {

        // Do an insert.
        if ($this->isDefaultRevision() and $this->isNewRevision()) {
          $tripal_storages[$tsid]->insertValues($tsid_values);
          $values[$tsid] = $tsid_values;
        }

        // Do an Update.
        else {
          $tripal_storages[$tsid]->updateValues($tsid_values);
          $values[$tsid] = $tsid_values;
        }

        // Note: We do not want to load empty fields!
        // There are some cases where empty fields sneak through to this point.
        // For example, if an empty element wasn't properly removed, it can get
        // through to here :see_no_evil: and if it does, chado storage will
        // helpfully do the best it can with limited filter criteria :cower:
        // often duplicating a single chado record into 2 drupal field records!
        // Rather then assume this will never happen, lets filter them out here
        // to reduce the impact if erroneous empty fields do get through.

        // @todo FILTER THEM
        // @see isEmptyFieldItem()??
        // @see $this->tripalfield_info[$field_name]['store-properties']


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
        foreach ($deltas as $delta) {
          // @debug print "Removing $field_name [ $delta ]...\n";
          $this->get($field_name)->removeItem($delta);
        }
      }
    }
    catch (\Exception $e) {
      \Drupal::logger('tripal')->error($e->getMessage());
      \Drupal::messenger()->addError("Cannot $mode_string this entity. See the recent logs for more details or contact the site administrator if you cannot view the logs.");
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
