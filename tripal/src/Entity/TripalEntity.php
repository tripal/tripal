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
use Symfony\Component\Routing\Route;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use \Drupal\tripal\Services\TripalTokenParser;

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
   * Constructs a new Tripal entity object, without permanently saving it.
   *
   * @code
      $values = [
        'title' => 'laceytest'.time(),
        'type' => 'organism',
        'uid' => 1,
      ];
      $entity = \Drupal\tripal\Entity\TripalEntity::create($values);
      $entity->save();
   * @endcode
   *
   * @param array $values
   *   - *title: the title of the entity.
   *   - *user_id: the user_id of the user who authored the content.
   *   - *type: the type of tripal entity this is (e.g. organism)
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
      'uid' => \Drupal::currentUser()->id(),
    );
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
    return $this->getTitle();
  }

  /**
   * {@inheritdoc}
   */
  public function setTitle($title = NULL, $cache = []) {

    // Get the bundle object.
    if (isset($cache['bundle'])) {
      $bundle = $cache['bundle'];
    }
    else {
      $bundle = \Drupal\tripal\Entity\TripalEntityType::load($this->getType());
    }

    // Get the values of the current entity.
    $entity_values = $this->getFieldValues();

    // Use the token parser directly.
    $title = TripalTokenParser::getEntityTitle($bundle, $entity_values);
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
   */
  public function setAlias($path_alias = NULL) {


    $system_path = "/bio_data/" . $this->getID();

    // If no alias was supplied then we should try to generate one using the
    // default format set by admins.
    if (!$path_alias) {

      $bundle = \Drupal\tripal\Entity\TripalEntityType::load($this->getType());
      $path_alias = $bundle->getURLFormat();
      $path_alias = $this->replaceTokens($path_alias, $bundle);
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
   * Retrieves the values of the current entity as a nested array.
   *
   * @return array
   *  This is a nested array with the first keys being field names. Within each
   *  array for a given field the keys are delta and the values are an array of
   *  the property names => values for that field delta.
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
        if (is_array($props)) {
          foreach ($props as $prop) {
            $values[$field_name][$delta][$prop->getName()] = $prop->getValue();
          }
        }
      }
    }
    return $values;
  }

  /**
   * Replaces tokens in a given tokens in URL Aliases and Titles.
   *
   * @param string $string
   *   The string to replace.
   * @param array $cache
   */
  protected function replaceTokens($string, $bundle) {

    // Initialize the Tripal token parser service.
    /** @var \Drupal\tripal\Services\TripalTokenParser $token_parser **/
    $token_parser = \Drupal::service('tripal.token_parser');
    $token_parser->initParser($bundle, $this);
    $field_defs = $this->getFieldDefinitions();
    foreach ($field_defs as $field_name => $field_def) {
      /** @var \Drupal\Core\Field\FieldItemList $items **/
      $items = $this->get($field_name);
      if ($items->count() == 1) {
        /** @var \Drupal\Core\Field\Plugin\Field\FieldType\EntityReferenceItem  $item **/
        /** @var \Drupal\Core\TypedData\TypedDataInterface $prop **/
        $item = $items->get(0);
        if (! $item instanceof TripalFieldItemBase) {
          continue;
        }
        $props = $item->getProperties();
        foreach ($props as $prop) {
          $token_parser->addFieldValue($field_name, $prop->getName(), $prop->getValue());
        }
      }
    }
    $replaced = $token_parser->replaceTokens([$string]);
    return $replaced[0];
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

    $fields['uid'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Authored by'))
      ->setDescription(t('The user ID of the author of the Tripal Content entity.'))
      ->setRevisionable(TRUE)
      ->setSetting('target_type', 'user')
      ->setSetting('handler', 'default')
      ->setTranslatable(TRUE)
      ->setDisplayOptions('view', array(
        'label' => 'above',
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
   *   - 'value': the property value object.
   *   - 'operation': the operation to use when matching this value.
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


        // If the Tripal Storage Backend is not set on a Tripal-based field,
        // we will log an error and not support the field. If developers want
        // to use Drupal storage for a Tripal-based field then they need to
        // indicate that by using our Drupal SQL Storage option OR by not
        // creating a Tripal-based field at all depending on their needs.
        if (empty($tsid)) {
          \Drupal::logger('tripal')->error('The Tripal-based field :field on
            this content type must indicate a TripalStorage backend and currently does not.',
            [':field' => $field_name]
          );
          continue;
        }

        // Create instance of the storage plugin so we can add the properties
        // to it as we go.
        if (!array_key_exists($tsid, $tripal_storages)) {
          $tripal_storage = \Drupal::service("tripal.storage")->getInstance(['plugin_id' => $tsid]);
          $tripal_storages[$tsid] = $tripal_storage;
        }

        // Add the field definition to the storage for this field.
        $tripal_storages[$tsid]->addFieldDefinition($field_name, $item->getFieldDefinition());

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
    // else will stay in the underlying data store (e.g. Chado).
    $delta_remove = [];
    $fields = $this->getFields();
    foreach ($fields as $field_name => $items) {
      foreach($items as $item) {

        // If it is not a TripalField then skip it.
        if (!($item instanceof TripalFieldItemInterface)) {
          continue;
        }

        $delta = $item->getName();
        $tsid = $item->tripalStorageId();

        // If the Tripal Storage Backend is not set on a Tripal-based field,
        // we will log an error and not support the field. If developers want
        // to use Drupal storage for a Tripal-based field then they need to
        // indicate that by using our Drupal SQL Storage option OR by not
        // creating a Tripal-based field at all depending on their needs.
        if (empty($tsid)) {
          \Drupal::logger('tripal')->error('The Tripal-based field :field on
            this content type must indicate a TripalStorage backend and currently does not.',
            [':field' => $field_name]
          );
          continue;
        }

        // Load into the entity the properties that are to be stored in Drupal.
        $prop_values = [];
        $prop_types = [];
        foreach ($values[$tsid][$field_name][$delta] as $key => $prop_info) {
          $prop_type = $tripal_storages[$tsid]->getPropertyType($field_name, $key);
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
          foreach ($prop_values as $i => $prop_value) {
            $prop_value_value = $prop_value->getValue();
            if (is_null($prop_value_value)) {
              // A given delta should only be present once here.
              if (!array_key_exists($field_name, $delta_remove) or !in_array($delta, $delta_remove[$field_name])) {
                $delta_remove[$field_name][] = $delta;
              }
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


    // IF we are doing a listing of content types there is no way in Drupal 10
    // to specify which fields to load.  By deafult the SqlContentEntityStorage
    // storage system we're using will always attach all fields.  But we can
    // control what fields get attached to entities with this postLoad function.
    // In the TripalEntityListBuilder::load() function we set the
    // `tripal_load_listing` session variable to TRUE.  If it is TRUE then
    // we skip this. @todo: in the future if we want to only attach
    // specific fields we can get more fancy.
    if (\Drupal::request()->hasSession()) {
      $session = \Drupal::request()->getSession();
      $is_listing = $session->get('tripal_load_listing');
      if ($is_listing === TRUE) {
        return;
      }
    }

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

          // If the Tripal Storage Backend is not set on a Tripal-based field,
          // we will log an error and not support the field. If developers want
          // to use Drupal storage for a Tripal-based field then they need to
          // indicate that by using our Drupal SQL Storage option OR by not
          // creating a Tripal-based field at all depending on their needs.
          if (empty($tsid)) {
            \Drupal::logger('tripal')->error('The Tripal-based field :field on
              this content type must indicate a TripalStorage backend and currently does not.',
              [':field' => $field_name]
            );
            continue;
          }

          // Create a new properties array for this field item.
          $prop_values = [];
          $prop_types = [];
          foreach ($values[$tsid][$field_name][$delta] as $key => $info) {
            $prop_values[] = $info['value'];
            $prop_types[] = $tripal_storages[$tsid]->getPropertyType($bundle, $field_name, $key);
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
    list($values, $tripal_storages) = TripalEntity::getValuesArray($this);

    // Call the deleteValues() function for each storage type.
    $delete_success = False;
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
