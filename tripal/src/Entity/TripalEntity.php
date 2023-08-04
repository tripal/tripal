<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\user\UserInterface;
use Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface;
use Drupal\field\Entity\FieldConfig;


/**
 * Defines the Tripal Content entity.
 *
 * @ingroup tripal
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
 *     "collection" = "/admin/content/bio_data",
 *   },
 *   bundle_entity_type = "tripal_entity_type",
 *   field_ui_base_route = "entity.tripal_entity_type.edit_form"
 * )
 */
class TripalEntity extends ContentEntityBase implements TripalEntityInterface {

  use EntityChangedTrait;


  /**
   * Constructs a new Tripal entity object, without permanently saving it.
   *
   * @code
      $values = [
        'title' => 'laceytest'.time(),
        'type' => 'bio_data_1',
        'uid' => 1,
      ];
      $entity = \Drupal\tripal\Entity\TripalEntity::create($values);
      $entity->save();
   * @endcode
   *
   * @param array $values
   *   - *title: the title of the entity.
   *   - *user_id: the user_id of the user who authored the content.
   *   - *type: the type of tripal entity this is (e.g. bio_data_1)
   *   - status: whether the entity is published or not (boolean)
   *   - created: the unix timestamp for when this content was created.
   * @return object
   *  The newly created entity.
   */
  public static function create(array $values = []) {
    return parent::create($values);
  }

  /**
   * {@inheritdoc}
   */
  public static function preCreate(EntityStorageInterface $storage_controller, array &$values) {
    parent::preCreate($storage_controller, $values);
    $values += array(
      'user_id' => \Drupal::currentUser()->id(),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getID() {
    $entity_id = $this->id();
    if (is_array($entity_id) AND array_key_exists(0, $entity_id)) {
      return $entity_id[0]['value'];
    }
    return $entity_id;
  }

  /**
   * {@inheritdoc}
   */
  public function label() {
    return $this->getTitle();
  }

  /**
   * {@inheritdoc}
   */
  public function setTitle($title = NULL, $cache = []) {

    if (isset($cache['bundle'])) {
      $bundle = $cache['bundle'];
    }
    else {
      $bundle = \Drupal\tripal\Entity\TripalEntityType::load($this->getType());
    }

    // If no title was supplied then we should try to generate one using the
    // default format set by admins.
    // @todo figure out how to override the title while still allowing
    //   tokenized titles to be updated on edit.
    $title = $bundle->getTitleFormat();
    $title = $this->replaceTokens($title, ['tripal_entity_type' => $bundle]);

    $this->title = $title;
  }

  /**
   * {@inheritdoc}
   */
  public function getTitle() {
    return $this->title->getString();
  }

  /**
   * Sets the URL alias for the current entity.
   *
   * @param string $alias
   *   The alias to use. It can contain tokens the correspond to field values.
   *   Token should be be compatible with those returned by
   *   tripal_get_entity_tokens().
   * @param array $cache
   *   This array is used to store objects you want to cache for performance
   *   reasons, as well as, cache related options. The following are supported:
   *   - TripalEntityType $bundle
   *       The bundle for the current entity.
   */
  public function setAlias($path_alias = NULL, $cache = []) {

    $system_path = "/bio_data/" . $this->getID();
    $langcode = \Drupal::languageManager()->getCurrentLanguage()->getId();

    // If no alias was supplied then we should try to generate one using the
    // default format set by admins.
    if (!$path_alias) {

      // Load the TripalEntityType entity for this TripalEntity (if it's not
      // cached). First get the format for the url alias based on the bundle
      // of the entity. Then replace all the tokens with values from the entity fields.
      if (isset($cache['bundle'])) {
        $bundle = $cache['bundle'];
      }
      else {
        $bundle = \Drupal\tripal\Entity\TripalEntityType::load($this->getType());
      }

      $path_alias = $bundle->getURLFormat();
      $path_alias = $this->replaceTokens($path_alias,
        ['tripal_entity_type' => $bundle]);

    }

    // Ensure there is a leading slash.
    if ($path_alias[0] != '/') {
      $path_alias = '/' . $path_alias;
    }

    // Make sure the path alias is URL friendly.
    $path_alias = str_replace(['%2F', '+'], ['/', '-'], urlencode($path_alias));

    // Now finally, set the alias.
    $path = \Drupal::entityTypeManager()->getStorage('path_alias')->create([
      'path' => $system_path,
      'alias' => $path_alias,
      'langcode' => $langcode,
    ]);
    $path->save();
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
    $this->set('status', $published ? NODE_PUBLISHED : NODE_NOT_PUBLISHED);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function replaceTokens($string, $cache = []) {

    // Pull any items out of the cache.
    if (isset($cache['bundle'])) {
      $bundle_entity = $cache['bundle'];
    }
    else {
      $bundle_entity = \Drupal\tripal\Entity\TripalEntityType::load($this->getType());
    }

    // Determine which tokens were used in the format string
    $used_tokens = [];
    if (preg_match_all('/\[.*?\]/', $string, $matches)) {
      $used_tokens = $matches[0];
    }

    // If there are no tokens then just return the string.
    if (count($used_tokens) == 0) {
      return $string;
    }

    // @todo UPGRADE THIS CODE ONCE TRIPAL FIELDS HAVE BEEN ADDED
    //
    // If the fields are not loaded for the entity then we want to load them
    // but we won't do a field_attach_load() as that will load all of the
    // fields. For syncing (publishing) of content loading all fields for
    // all synced entities causes extreme slowness, so we'll only attach
    // the necessary fields for replacing tokens.
    // $attach_fields = [];
    //
    // foreach ($used_tokens as $token) {
    //   $token = preg_replace('/[\[\]]/', '', $token);
    //   $elements = explode(',', $token);
    //   $field_name = array_shift($elements);
    //
    //   if (!property_exists($entity, $field_name) or empty($entity->{$field_name})) {
    //     $field = field_info_field($field_name);
    //     $storage = $field['storage'];
    //     $attach_fields[$storage['type']]['storage'] = $storage;
    //     $attach_fields[$storage['type']]['fields'][] = $field;
    //   }
    // }
    //
    // // If we have any fields that need attaching, then do so now.
    // if (count(array_keys($attach_fields)) > 0) {
    //   foreach ($attach_fields as $storage_type => $details) {
    //     $field_ids = [];
    //     $storage = $details['storage'];
    //     $fields = $details['fields'];
    //     foreach ($fields as $field) {
    //       $field_ids[$field['id']] = [$entity->id];
    //     }
    //     $entities = [$entity->id => $entity];
    //     module_invoke($storage['module'], 'field_storage_load', 'TripalEntity',
    //       $entities, FIELD_LOAD_CURRENT, $field_ids, []);
    //   }
    // }

    // Now that all necessary fields are attached process the tokens.
    foreach ($used_tokens as $token) {
      $token = preg_replace('/[\[\]]/', '', $token);
      $elements = explode(',', $token);
      $field_name = array_shift($elements);
      $value = '';

      // The TripalBundle__bundle_id is a special token for substituting the
      // bundle id.
      if ($token === 'TripalBundle__bundle_id') {
        // This token should be the id of the TripalBundle.
        $value = $bundle_entity->getID();
      }
      // The TripalBundle__bundle_id is a special token for substituting the
      // entity id.
      elseif ($token === 'TripalEntity__entity_id') {
        // This token should be the id of the TripalEntity.
        $value = $this->getID();
      }
      elseif ($token == 'TripalEntityType__label') {
        $value = $bundle_entity->getLabel();
      }
      else {
        $value_obj = $this->get($field_name);
        if ($value_obj) {
          // A field may have multiple properties. We should use the
          // main property key for the field when replacing the value.
          $field_type = $this->getFieldDefinition($field_name)->getType();
          $field_types = \Drupal::service('plugin.manager.field.field_type');
          $field_type_def = $field_types->getDefinition($field_type);
          $field_class = $field_type_def['class'];
          $main_key = $field_class::mainPropertyName();

          // Get the value if the property has one.
          $value_raw = $value_obj->getValue();
          $value = '';
          if (array_key_exists($main_key, $value_raw[0])) {
            $value = $value_raw[0][$main_key];
          }

          // If the value is an array it means we have sub elements and we can
          // descend through the array to look for matching value.
          if (is_array($value) and count($elements) > 0) {
            // @todo we still need to handle this case.
            $value = '';
            //$value = _tripal_replace_entity_tokens_for_elements($elements, $value);
          }
        }
      }

      // We can't support tokens that have multiple elements (i.e. in an array).
      if (is_array($value)) {
        $string = str_replace('[' . $token . ']', '', $string);
      }
      else {
        $string = str_replace('[' . $token . ']', $value ?? '', $string);
      }
    }

    return $string;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

    $fields['uid'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Authored by'))
      ->setDescription(t('The user ID of author of the Tripal Content entity.'))
      ->setRevisionable(TRUE)
      ->setSetting('target_type', 'user')
      ->setSetting('handler', 'default')
      ->setTranslatable(TRUE)
      ->setDisplayOptions('view', array(
        'label' => 'hidden',
        'type' => 'author',
        'weight' => 0,
      ))
      ->setDisplayOptions('form', array(
        'type' => 'entity_reference_autocomplete',
        'weight' => 5,
        'settings' => array(
          'match_operator' => 'CONTAINS',
          'size' => '60',
          'autocomplete_type' => 'tags',
          'placeholder' => '',
        ),
      ))
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['title'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Title'))
      ->setDescription(t('The title of this entity.'))
      ->setSettings(array(
        'max_length' => 1024,
        'text_processing' => 0,
      ))
      ->setDefaultValue('')
      ->setDisplayOptions('view', array(
        'label' => 'above',
        'type' => 'string',
        'weight' => -4,
      ))
      ->setDisplayOptions('form', array(
        'type' => 'string_textfield',
        'weight' => -4,
      ))
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['status'] = BaseFieldDefinition::create('boolean')
      ->setLabel(t('Publishing status'))
      ->setDescription(t('A boolean indicating whether the Tripal Content is published.'))
      ->setDefaultValue(TRUE);

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Created'))
      ->setDescription(t('The time that the entity was created.'));

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The time that the entity was last edited.'));

    return $fields;
  }

  /**
   * Returns an associative array of property type value for the entity.
   *
   * The array is keyed in the following levels:
   * - 1st: Tripal Stroage Plugin ID
   * - 2nd: Field name
   * - 3rd: Delta value of the field item.
   * - 4th: the property key.
   * - 5th: One of the following keys:
   *   - 'type': The property type object.
   *   - 'definition':  the field definition object for the field that this
   *     property belongs to.
   *   - 'value': the property value object.
   *
   * This function also returns an array of TripalStorage objects.
   *
   * @param TripalEntity $entity
   *
   * @return array
   *   The returned array has two elements: an array of values as described
   *   above, and an array of TripalStorage objects,
   */
  public static function getValuesArray($entity) {
    $values = [];
    $tripal_storages = [];
    $fields = $entity->getFields();

    // Specifically, for each field...
    foreach ($fields as $field_name => $items) {
      foreach($items as $item) {

        // If it is not a TripalField then skip it.
        if (! $item instanceof TripalFieldItemInterface) {
          continue;
        }

        $delta = $item->getName();
        $tsid = $item->tripalStorageId();


        // Create instance of the storage plugin so we can add the properties
        // to it as we go.
        if (!array_key_exists($tsid, $tripal_storages)) {
          $tripal_storage = \Drupal::service("tripal.storage")->getInstance(['plugin_id' => $tsid]);
          $tripal_storages[$tsid] = $tripal_storage;
        }

        // Get the empty property values for this field item and the
        // property type objects.
        $prop_values = $item->tripalValuesTemplate($item->getFieldDefinition());
        $prop_types = get_class($item)::tripalTypes($item->getFieldDefinition());

        // Sets the values from the entity on both the property and in entity.
        // Despite the function name, no values are saved to the database.
        $item->tripalSave($item, $field_name, $prop_types, $prop_values, $entity);

        // Clears the values from the entity (does not clear them from the
        // property).
        $item->tripalClear($item, $field_name, $prop_types, $prop_values, $entity);

        // Add the property types to the storage plugin.
        $tripal_storages[$tsid]->addTypes($entity->getType(), $field_name, $prop_types);

        // Prepare the property values for the storage plugin.
        // Note: We are assuming the key for the value is the
        // same as the key for the type here... This is a temporary assumption
        // as soon the values array will not contain types ;-)
        foreach ($prop_types as $prop_type) {
          $key = $prop_type->getKey();
          $values[$tsid][$field_name][$delta][$key] = [
            'definition' => $item->getFieldDefinition(),
            'type' => $prop_type
          ];
        }
        foreach ($prop_values as $prop_value) {
          $key = $prop_value->getKey();
          $values[$tsid][$field_name][$delta][$key]['value'] = $prop_value;
        }
      }
    }
    return [$values, $tripal_storages];
  }

  /**
   * {@inheritdoc}
   */
  public function preSave(EntityStorageInterface $storage) {
    parent::preSave($storage);

    // Create a values array appropriate for `loadValues()`
    list($values, $tripal_storages) = TripalEntity::getValuesArray($this);

    // Perform the Insert or Update of the submitted values to the
    // underlying data store.
    foreach ($values as $tsid => $tsid_values) {

      // Do an insert
      if ($this->isDefaultRevision() and $this->isNewRevision()) {
        try {
          $tripal_storages[$tsid]->insertValues($tsid_values);
        }
        catch (\Exception $e) {
          \Drupal::logger('tripal')->notice($e->getMessage());
          \Drupal::messenger()->addError('Cannot insert this entity. See the recent ' .
              'logs for more details or contact the site administrator if you ' .
              'cannot view the logs.');
        }
        $values[$tsid] = $tsid_values;
      }

      // Do an Update
      else {
        try {
          $tripal_storages[$tsid]->updateValues($tsid_values);
        }
        catch (\Exception $e) {
          \Drupal::logger('tripal')->notice($e->getMessage());
          \Drupal::messenger()->addError('Cannot update this entity. See the recent ' .
              'logs for more details or contact the site administrator if you cannot ' .
              'view the logs.');
        }
      }
    }

    // Set the property values that should be saved in Drupal, everything
    // else will stay in the underlying data store (e.g. Chado)..
    $delta_remove = [];
    $bundle_name = $this->getType();
    $fields = $this->getFields();
    foreach ($fields as $field_name => $items) {
      foreach($items as $item) {

        // If it is not a TripalField then skip it.
        if (!($item instanceof TripalFieldItemInterface)) {
          continue;
        }

        $delta = $item->getName();
        $tsid = $item->tripalStorageId();

        // Load into the entity the properties that are to be stored in Drupal.
        $prop_values = [];
        $prop_types = [];
        foreach ($values[$tsid][$field_name][$delta] as $key => $prop_info) {
          $prop_type = $tripal_storages[$tsid]->getType($bundle_name, $field_name, $key);
          $prop_value = $prop_info['value'];
          $settings = $prop_type->getStorageSettings();
          if (array_key_exists('drupal_store', $settings) and $settings['drupal_store'] == TRUE) {
            $prop_values[] = $prop_value;
            $prop_types[] = $prop_type;
          }
        }
        if (count($prop_values) > 0) {
          $item->tripalLoad($item, $field_name, $prop_types, $prop_values, $this);

          // Keep track of elements that have no value.
          foreach ($prop_values as $prop_value) {
            if (!$prop_value->getValue()) {
              $delta_remove[$field_name][] = $delta;
              continue;
            }
          }
        }
      }
    }

    // Now remove any values that shouldn't be there.
    foreach ($delta_remove as $field_name => $deltas) {
      foreach (array_reverse($deltas) as $delta) {
        try {
          $this->get($field_name)->removeItem($delta);
        }
        catch (\Exception $e) {
          \Drupal::logger('tripal')->notice($e->getMessage());
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
  public static function postLoad(EntityStorageInterface $storage, array &$entities) {
    parent::postLoad($storage, $entities);

    $entity_type_id = $storage->getEntityTypeId();
    $field_manager = \Drupal::service('entity_field.manager');
    $field_type_manager = \Drupal::service('plugin.manager.field.field_type');

    // Iterate through the entities provided.
    foreach ($entities as $entity) {
      $bundle = $entity->bundle();

      // Create a values array appropriate for `loadValues()`
      list($values, $tripal_storages) = TripalEntity::getValuesArray($entity);


      // Call the loadValues() function for each storage type.
      $load_success = False;
      foreach ($values as $tsid => $tsid_values) {
        try {
          $load_success = $tripal_storages[$tsid]->loadValues($tsid_values);
          if ($load_success) {
            $values[$tsid] = $tsid_values;
          }
        }
        catch (\Exception $e) {
          \Drupal::logger('tripal')->notice($e->getMessage());
          \Drupal::messenger()->addError('Cannot load the entity. See the recent ' .
              'logs for more details or contact the site administrator if you cannot ' .
              'view the logs.');
        }
      }

      // Update the entity values with the values returned by loadValues().
      $field_defs = $field_manager->getFieldDefinitions($entity_type_id, $bundle);
      foreach ($field_defs as $field_name => $field_def) {

        // Create a fieldItemlist and iterate through it.
        $items = $field_type_manager->createFieldItemList($entity, $field_name, $entity->get($field_name)->getValue());
        foreach($items as $item) {

          // If it is not a TripalField then skip it.
          if (! $item instanceof TripalFieldItemInterface) {
            continue;
          }
          $delta = $item->getName();
          $tsid = $item->tripalStorageId();

          // Create a new properties array for this field item.
          $prop_values = [];
          $prop_types = [];
          foreach ($values[$tsid][$field_name][$delta] as $key => $info) {
            $prop_values[] = $info['value'];
            $prop_types[] = $tripal_storages[$tsid]->getType($bundle, $field_name, $key);
          }

          // Now set the entity values for this field.
          $item->tripalLoad($item, $field_name, $prop_types, $prop_values, $entity);
        }
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function validate() {
    // Let the parent class do its validations and return the violations list.
    $violations = parent::validate();

    // Create a values array appropriate for `loadValues()`
    list($values, $tripal_storages) = TripalEntity::getValuesArray($this);

    // Iterate through the different Tripal Storage objects and run the
    // validateVales() function for the values that belong to it.
    foreach ($values as $tsid => $tsid_values) {
      $problems = $tripal_storages[$tsid]->validateValues($tsid_values);
      foreach ($problems as $violation) {
        $violations->add($violation);
      }
    }

    return $violations;
  }
}
