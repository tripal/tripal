<?php

namespace Drupal\tripal_chado\Plugin\TripalVocabulary;

use Drupal\tripal\TripalVocabTerms\TripalVocabularyBase;
use Drupal\tripal\Services\TripalLogger;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Chado implementation of the TripalVocabularyBase.
 *
 *  @TripalVocabulary(
 *    id = "chado_vocabulary",
 *    label = @Translation("Vocabulary in Chado"),
 *  )
 */
class ChadoVocabulary extends TripalVocabularyBase implements ContainerFactoryPluginInterface {

  /**
   * The definition for the `db` table of Chado.
   *
   * @var array
   */
  protected $db_def = NULL;

  /**
   * The definition for the `cv` table of Chado.
   *
   * @var array
   */
  protected $cv_def = NULL;

  /**
   * The logger for reporting warnings and errors to admin.
   *
   * @var \Drupal\tripal\Services\TripalLogger
   */
  protected $messageLogger = NULL;

  /**
   * The database connection for querying Chado.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected $connection;

  /**
   * A simple boolean to prevent Chado queries if the vocabulary isn't valid.
   *
   * @var bool
   */
  protected $is_valid = False;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static function
   * will be called behind the scenes when a Plugin Manager uses createInstance(). Specifically
   * this method is used to determine the parameters to pass to the contructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   *
   * @return static
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {

    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('tripal.logger'),
      $container->get('tripal_chado.database')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, TripalLogger $logger, ChadoConnection $connection) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);

    $this->messageLogger = $logger;
    $this->connection = $connection;

    // Get the chado definition for the `cv` and `db` tables.
    $this->db_def = $this->connection->schema()->getTableDef('db', ['source' => 'file']);
    $this->cv_def = $this->connection->schema()->getTableDef('cv', ['source' => 'file']);
  }


  /**
   * {@inheritdoc}
   */
  public function isValid() {

    // Make sure the name of this collection does not exceeed the allowed size in Chado.
    $name = $this->getName();
    if (!empty($name) AND (strlen($name) > $this->cv_def['fields']['name']['size'])) {
      $this->messageLogger->error('ChadoVocabulary: The vocabulary name must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => $this->cv_def['fields']['name']['size'],
           '@value' => $this->getName()]);
      $this->is_valid = FALSE;
      return FALSE;

    }
    $this->is_valid = TRUE;
    return TRUE;
  }

  /**
   * {@inheritdoc}
   */
  public function recordExists() {
   $cv = $this->loadVocab();
   if ($cv and $cv['name'] == $this->getName()) {
      return True;
    }
    return False;
  }

  /**
   * {@inheritdoc}
   */
  public function createRecord(){

    // Check if the record already exists in the database, if it
    // doesn't then insert it.  We don't yet have the definition,
    // but that's okay, the name is all that isrequired to create
    // a record in the `cv` table.
    $vocab = $this->loadVocab();
    if (!$vocab) {
      $query = $this->connection->insert('1:cv')
        ->fields(['name' => $this->getName()]);
      $query->execute();
    }
  }

  /**
   * {@inheritdoc}
   */
  public function destroy(){
    // The destroy function is meant to delete the vocabulary.
    // But, because CVs and DBs are so critical to almost all
    // data in Chado we don't want to remove the records.
    // Let's let the collection be deleted as far as
    // Tripal is concerned but leave the record in Chado.
    // So, do nothing here.
    $this->messageLogger->warning('The ChadoVocabulary::destroy() function is currently not implemented');
  }


  /**
   * Loads a Vocabulary record from Chado.
   *
   * This function queries the `cv` table of Chado to get the values
   * for the vocabulary.
   *
   * @return
   *   An associative array containing the columns of the `db1 table
   *   of Chado or NULL if the db could not be found.
   */
  protected function loadVocab() {

    // Get the Chado `db` record for this ID space.
    $query = $this->connection->select('1:cv', 'cv')
      ->condition('cv.name', $this->getName(), '=')
      ->fields('cv', ['name', 'definition']);
    $result = $query->execute();
    if ($result) {
      return $result->fetchAssoc();
    }
    return NULL;
  }

  /**
   * {@inheritdoc}
   */
  public function getIdSpaceNames(){
    return $this->getIdSpacesCache();
  }

  /**
   * Returns the IDSpace cache ID.
   *
   * @return string
   */
  protected function getIdSpaceCacheID() {

    $idSpace = $this->getName();
    $chado_schema = $this->connection->getSchemaName();
    return 'chado_vocabulary_' . $chado_schema . '_' . $idSpace . '_id_spaces';
  }

  /**
   * Sets the ID spaces for this vocabulary in the Drupal cache.
   *
   * The current way to map CV's to DB's is to use the `cv2db`
   * materialized view but there is no guarantee that that mview
   * is up-to-date and it would take too long to force an update
   * every time we need to get the ID spaces for a vocabulary.  This
   * function caches it.
   *
   * @param $id_spaces
   *   An array containing the names of the ID spaces.
   */
  protected function setIdSpacesCache($id_spaces) {
    $cid = $this->getIdSpaceCacheID();
    \Drupal::cache()->set($cid, $id_spaces);
  }

  /**
   * Retrieves from the Drupal cache the ID spaces of this vocabulary.
   *
   * The current way to map CV's to DB's is to use the `cv2db`
   * materialized view but there is no guarantee that that mview
   * is up-to-date and it would take too long to force an update
   * every time we need to get the ID spaces for a vocabulary.  This
   * function retrieves the ID spaces for a vocabulary from a
   * Drupal cache.
   *
   * @return array
   *   An array of ID Space names.
   */
  protected function getIdSpacesCache() {
    $cid = $this->getIdSpaceCacheID();
    $id_spaces = [];
    if ($cache = \Drupal::cache()->get($cid)) {
      $id_spaces = $cache->data;
    }
    return $id_spaces;
  }


  /**
   * {@inheritdoc}
   */
  public function addIdSpace($idSpace){

    // Get the ID collection for this idSpace and save it for future
    // reference, then add the idSpace to our list.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $id = $idsmanager->loadCollection($idSpace);
    if ($id) {
      $id_spaces = $this->getIdSpacesCache();
      $id_spaces[] = $idSpace;
      $this->setIdSpacesCache($id_spaces);
      return True;
    }
    return False;
  }


  /**
   * {@inheritdoc}
   */
  public function removeIdSpace($idSpace){
    $id_spaces = $this->getIdSpacesCache();
    $new_ids = [];
    $found = False;
    foreach ($id_spaces as $name) {
      if ($name != $idSpace) {
        $new_ids[] = $name;
      }
      else {
        $found = True;
      }
    }
    $this->setIdSpacesCache($new_ids);
    return $found;
  }

  /**
   * {@inheritdoc}
   */
  public function getTerms($name, $exact = True){
    $this->messageLogger->warning('The ChadoVocabulary::getTerms() function is currently not implemented');
  }

  /**
   * {@inheritdoc}
   */
  public function getURL(){
    // Don't get a value for a vocabulary that isn't valid.
    if (!$this->is_valid) {
      return NULL;
    }

    // If we don't have any ID spaces then there is no URL.
    $id_spaces = $this->getIdSpacesCache();
    if (count($id_spaces) == 0) {
      $this->messageLogger->error('ChadoVocabulary: Cannot get the URL when no ID spaces are present for the vocabulary.');
      return NULL;
    }

    // All of the ID spaces for the vocabulary should
    // have the same URL, so only query the first corresponding
    // `db` record to get the URL.
    $db = $this->connection->select('1:db', 'db')
      ->fields('db', ['url'])
      ->condition('db.name', $id_spaces[0], '=')
      ->execute();
    if (!$db) {
      return NULL;
    }
    return $db->fetchField();
  }

  /**
   * {@inheritdoc}
   */
  public function setURL($url){

    // @todo there may be a problem in the future if we are able to
    // associate borrowed terms with a vocabulary in Chado.  If we
    // add the ID space of borrowed terms to a vocabulary then
    // setting the URL will be incorrect for those ID spaces.

    // Don't set a value for a vocabulary that isn't valid.
    if (!$this->is_valid) {
      return False;
    }

    // If we don't have any ID spaces then there is no URL.
    $id_spaces = $this->getIdSpacesCache();
    if (count($id_spaces) == 0) {
      $this->messageLogger->error('ChadoVocabulary: Cannot set the URL when no ID spaces are present for the vocabulary.');
      return False;
    }

    // This value goes to the Chado `db.url` column, so check its size
    // to make sure it doesn't exceed it.
    if (strlen($url) > $this->db_def['fields']['url']['size']) {
      $this->messageLogger->error('ChadoVocabulary: The vocabulary name must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => $this->cv_def['fields']['name']['size'],
            '@value' => $this->getName()]);
      return False;
    }

    // Update the record in the Chado `db` table for the URL for all ID spaces.
    foreach ($id_spaces as $name) {
      $num_updated = $this->connection->update('1:db')
        ->fields(['url' => $url])
        ->condition('name', $name, '=')
        ->execute();
      if ($num_updated != 1) {
        $this->messageLogger->error(t('ChadoVocabulary: The URL could not be updated for the vocabulary, "@vocab.',
          ['@vocab' => $this->getName()]));
        return False;
      }
    }
    return True;
  }

  /**
   * Returns the namespace of the vocabulary
   *
   * This should be identical to the name of the collection, and
   * therefore, there is no setter function.
   *
   * @return string $namespace
   *   The namespace of the vocabulary.
   */
  public function getNameSpace() {
    return $this->getName();
  }


  /**
   * {@inheritdoc}
   */
  public function setLabel($label) {
    // Don't set a value for a vocabulary that isn't valid.
    if (!$this->is_valid) {
      return False;
    }

    // Note: there's no need to check the size of the label value
    // because the Chado column where this goes (cv.definition) is an
    // unlimited text field.

    // Update the record in the Chado `cv` table.
    $query = $this->connection->update('1:cv')
      ->fields(['definition' => $label])
      ->condition('name', $this->getName(), '=');
    $num_updated = $query->execute();
    if ($num_updated != 1) {
      $this->messageLogger->error('ChadoVocabulary: The label could not be updated for the vocabulary.');
      return False;
    }
    return True;
  }


  /**
   * {@inheritdoc}
   */
  public function getLabel() {
    $cv = $this->loadVocab();
    if (!$cv) {
      return NULL;
    }
    return $cv['definition'];
  }
}
