<?php

namespace Drupal\tripal\TripalStorage\Interfaces;

use Drupal\Component\Plugin\PluginInspectionInterface;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Symfony\Component\HttpKernel\DependencyInjection\AddAnnotatedClassesToCachePass;
use Drupal\Core\Form\FormStateInterface;


/**
 * Defines an interface for tripal storage plugins.
 */
interface TripalStorageInterface extends PluginInspectionInterface {

  /**
   * Adds the given array of new property types to this tripal storage plugin.
   *
   * @param string $field_name
   *   The name of the field the properties belong to.
   * @param array $types
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase objects.
   */
  public function addTypes(string $field_name, array $types);

  /**
   * Removes the given array of property types from this tripal storage plugin.
   *
   * @param string $field_name
   *   The name of the field the properties belong to.
   * @param array $types
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase objects.
   */
  public function removeTypes(string $field_name, array $types);

  /**
   * Returns a list of all property types added to this storage plugin type.
   * WARING! This could be a very expensive call!
   *
   * @return array
   *   Array of all \Drupal\tripal\Base\StoragePropertyTypeBase objects that
   *   have been added to this storage plugin type.
   */
  public function getTypes();

  /**
   * Returns a single propertyType object based on the parameters.
   *
   * @param string $bundle_name
   *   The name of the bundle on which the field is attached that the properties
   *   belong to.
   * @param string $field_name
   *   The name of the field the properties belong to.
   * @param string $key
   *   The key of the property type to return.
   * @return object
   *   An instance of the propertyType indicated.
   */
  public function getPropertyType(string $field_name, string $key);

  /**
   * Stores the field definition for a given field.
   *
   * NOTE: the definition for every field mentioned in the values array
   * of an insert/update/load/find/deleteValues() method must be added
   * using this function before the *Values() method can be called.
   *
   * @param string $field_name
   *   The name of the field based on it's annotation 'id'.
   * @param object $field_definition
   *   The field configuration object. This can be an instance of:
   *   \Drupal\field\Entity\FieldStorageConfig or
   *   \Drupal\field\Entity\FieldConfig
   * @return boolean
   *   Returns true if no errors were encountered and false otherwise.
   */
  public function addFieldDefinition(string $field_name, object $field_definition);

  /**
   * Retrieves the stored field definition of a given field.
   *
   * @param string $field_name
   *   The name of the field based on it's annotation 'id'.
   * @return object $field_definition
   *   The field configuration object. This can be an instance of:
   *   \Drupal\field\Entity\FieldStorageConfig or
   *   \Drupal\field\Entity\FieldConfig
   */
  public function getFieldDefinition(string $field_name);

  /**
   * Returns a list of property types that should be stored.
   *
   * In order to link data in the storage backend, the storage
   * system must link the record in someway with Drupal entities.
   * This most likely happens in tables in the Drupal schema
   * (usually the `public` schema).  This function should return
   * the list of properties that must be stored in order
   * to uniquely identify an entity in the datastore.
   *
   * @return @array
   *   Array of \Drupal\tripal\Base\StoragePropertyTypeBase objects.
   */
  public function getStoredTypes();

  /**
   * Returns a list of property values for stored types..
   *
   * This function returns an array of property value objects that
   * correspond to the types returned by getStoredTypes().
   *
   * @return @array
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   */
  public function getStoredValues();

  /**
   * Inserts values in the field data store.
   *
   * The record Ids of the inserted records will be set in the property
   * value objects.
   *
   * @param array $values
   *   Associative array 5-levels deep.
   *   The 1st level is the field name (e.g. ChadoOrganismDefault).
   *   The 2nd level is the delta value (e.g. 0).
   *   The 3rd level is a field key name (i.e. record_id + value).
   *   The 4th level must contain the following three keys/value pairs
   *   - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *   - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *   - "definition": a \Drupal\Field\Entity\FieldConfig object
   *   When the function returns, any values retrieved from the data store
   *   will be set in the StoragePropertyValue object.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function insertValues(&$values) : bool;

  /**
   * Updates values in the field data store.
   *
   * @param array $values
   *   Associative array 5-levels deep.
   *   The 1st level is the field name (e.g. ChadoOrganismDefault).
   *   The 2nd level is the delta value (e.g. 0).
   *   The 3rd level is a field key name (i.e. record_id + value).
   *   The 4th level must contain the following three keys/value pairs
   *   - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *   - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *   - "definition": a \Drupal\Field\Entity\FieldConfig object
   *   When the function returns, any values retrieved from the data store
   *   will be set in the StoragePropertyValue object.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function updateValues(&$values) : bool;

  /**
   * Loads the values of the field data store.
   *
   * @param array $values
   *   Associative array 5-levels deep.
   *   The 1st level is the field name (e.g. ChadoOrganismDefault).
   *   The 2nd level is the delta value (e.g. 0).
   *   The 3rd level is a field key name (i.e. record_id + value).
   *   The 4th level must contain the following three keys/value pairs
   *   - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *   - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *   - "definition": a \Drupal\Field\Entity\FieldConfig object
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function loadValues(&$values) : bool;

  /**
   * Deletes the given array of property values from this tripal storage plugin.
   *
   * @param array $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function deleteValues($values) : bool;

  /**
   * Finds and returns all property values stored in this storage plugin
   * implementation that matches the given match argument.
   *
   * @param array $values
   *   Associative array 5-levels deep.
   *   The 1st level is the field name (e.g. ChadoOrganismDefault).
   *   The 2nd level is the delta value (e.g. 0).
   *   The 3rd level is a field key name (i.e. record_id + value).
   *   The 4th level must contain the following three keys/value pairs
   *   - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *   - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *   - "definition": a \Drupal\Field\Entity\FieldConfig object
   *   When the function returns, any values retrieved from the data store
   *   will be set in the StoragePropertyValue object.
   *
   * @return array
   *   @todo fix this return value...
   *   Array of all \Drupal\tripal\TripalStorage\StoragePropertyValue objects
   *   that match.
   */
  public function findValues($values);



  /**
   * Performs validation checks on values.
   *
   * @param array $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.
   *
   * @return array
   *   An array of \Symfony\Component\Validator\ConstraintViolation objects.
   */
  public function validateValues($values);


  /**
   * Provides form elements to be added to the Tripal entity publish form.
   *
   * @param array $form
   *   The form array definition.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   *
   * @return array
   *   A new form array definition containing the form elements to add
   *   to the publish form.
   */
  public function publishForm($form, FormStateInterface &$form_state);

  /**
   * Handles validation of the publish form elements.
   *
   * @param array $form
   *   The form array definition.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   *
   */
  public function publishFormValidate($form, FormStateInterface &$form_state);

  /**
   * Handles submission of the form elements for the storage backend.
   *
   * @param array $form
   *   The form array definition.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function publishFromSubmit($form, FormStateInterface &$form_state);


}
