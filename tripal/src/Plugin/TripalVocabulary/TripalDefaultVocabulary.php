<?php

namespace Drupal\tripal\Plugin\TripalVocabulary;

use Drupal\tripal\TripalVocabTerms\TripalVocabularyBase;

/**
 * Chado implementation of the TripalVocabularyBase.
 *
 *  @TripalVocabulary(
 *    id = "tripal_default_vocabulary",
 *    label = @Translation("Deafult Tripal Vocabulary Plugin"),
 *  )
 */
class TripalDefaultVocabulary extends TripalVocabularyBase {
  /**
   * An instance of the TripalLogger.
   *
   * @var \Drupal\tripal\Services\TripalLogger
   */
  protected $messageLogger = NULL;

  /**
   * A simple boolean to prevent Chado queries if the vocabulary isn't valid.
   *
   * @var bool
   */
  protected $is_valid = False;

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);

    // Instantiate the TripalLogger
    $this->messageLogger = \Drupal::service('tripal.logger');
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

    $conn = \Drupal::service('database');

    // Get the Chado `db` record for this ID space.
    $query = $conn->select('tripal_terms_vocabs', 'vocabs')
      ->condition('name', $this->getName(), '=')
      ->fields('vocabs', ['name', 'label', 'url', 'idspaces']);
    $result = $query->execute();
    if ($result) {
      return $result->fetchAssoc();
    }
    return NULL;
  }

  /**
   * {@inheritDoc}
   */
  public function getIdSpaceNames() {
    $cv = $this->loadVocab();
    $idspaces = unserialize($cv['idspaces']);
    return $idspaces;
  }

  /**
   * {@inheritDoc}
   */
  public function getLabel() {
    $cv = $this->loadVocab();
    return $cv['label'];
  }

  /**
   * {@inheritDoc}
   */
  public function recordExists() {
  }

  /**
   * {@inheritDoc}
   */
  public function isValid() {

    // Make sure the name of this collection does not exceeed the allowed size in Chado.
    $name = $this->getName();
    if (!empty($name) AND (strlen($name) > 255)) {
      $this->messageLogger->error('TripalDefaultVocabular: The vocabulary name must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => 255, '@value' => $this->getName()]);
    }
    $this->is_valid = TRUE;

    return $this->is_valid;
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
   * {@inheritDoc}
   */
  public function destroy() {
  }

  /**
   * {@inheritDoc}
   */
  public function setURL($url) {
    // @todo there may be a problem in the future if we are able to
    // associate borrowed terms with a vocabulary.  If we
    // add the ID space of borrowed terms to a vocabulary then
    // setting the URL will be incorrect for those ID spaces.

    // Don't set a value for a vocabulary that isn't valid.
    if (!$this->is_valid) {
      return False;
    }

    // This value goes to the Chado `db.url` column, so check its size
    // to make sure it doesn't exceed it.
    if (strlen($url) > 255) {
      $this->messageLogger->error('TripalDefaultVocabulary: The vocabulary name must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => 255, '@value' => $this->getName()]);
          return False;
    }

    // If we don't have any ID spaces then there is no URL.
    $id_spaces = $this->getIdSpaceNames();
    if (count($id_spaces) == 0) {
      $this->messageLogger->error('TripalDefaultVocabulary: Cannot set the URL when no ID spaces are present for the vocabulary.');
      return False;
    }

    // Update the record in the Chado `db` table for the URL for all ID spaces.
    $conn = \Drupal::service('database');
    $update = $conn->update('tripal_terms_vocabs');
    $update = $update->fields(['url' => $url]);
    $update = $update->condition('name', $this->getName(), '=');
    $num_updated = $update->execute();
    if ($num_updated != 1) {
      $this->messageLogger->error(t('TripalDefaultVocabulary: The URL could not be updated for the vocabulary, "@vocab.',
          ['@vocab' => $this->getName()]));
      return False;
    }
    return True;
  }

  /**
   * {@inheritDoc}
   */
  public function getURL() {
    $cv = $this->loadVocab();
    return $cv['url'];
  }

  /**
   * {@inheritDoc}
   */
  public function setLabel($label) {
    // Don't set a value for a vocabulary that isn't valid.
    if (!$this->is_valid) {
      return False;
    }

    // Make sure the label is not too long.
    if (empty($label)) {
      $this->messageLogger->error('TripalDeafultVocabulary: You must provide a label when calling setLabel().',
          ['@size' => 255, '@value' => $label]);
      return False;
    }
    if (strlen($label) > 255) {
      $this->messageLogger->error('TripalDeafultVocabulary: The label for the vocabulary must not be longer than @size characters. ' +
          'The value provided was: @value',
          ['@size' => 255, '@value' => $label]);
      return False;
    }


    // Update the record in the Chado `cv` table.
    $chado = \Drupal::service('database');
    $update = $chado->update('tripal_terms_vocabs');
    $update = $update->fields(['label' => $label]);
    $update = $update->condition('name', $this->getName(), '=');
    $num_updated = $update->execute();
    if ($num_updated != 1) {
      $this->logInvalidCondition('TripalDeafultVocabulary: The label could not be updated for the vocabulary.');
      return False;
    }
    return True;
  }

  /**
   * {@inheritDoc}
   */
  public function create() {

    // Check if the record already exists in the database, if it
    // doesn't then insert it.  We don't yet have the definition,
    // but that's okay, the name is all that isrequired to create
    // a record in the `cv` table.
    $conn = \Drupal::service('database');
    $vocab = $this->loadVocab();
    if (!$vocab) {
      $query = $conn->insert('tripal_terms_vocabs');
      $query = $query->fields([
        'name' => $this->getName(),
        'idspaces' => serialize([]),
      ]);
      $query->execute();
    }
  }

  /**
   * {@inheritDoc}
   */
  public function addIdSpace($idSpace) {

    $idspaces = $this->getIdSpaceNames();
    $idspaces[] = $idSpace;

    $conn = \Drupal::service('database');
    $update = $conn->update('tripal_terms_vocabs');
    $update = $update->fields(['idspaces' => serialize($idspaces)]);
    $update = $update->condition('name', $this->getName(), '=');
    $update->execute();
  }

  /**
   * {@inheritDoc}
   */
  public function removeIdSpace($idSpace) {
    $idspaces = $this->getIdSpaceNames();
    if (in_array($idSpace, $idspaces)) {
      $idspaces = array_diff($idspaces, [$idSpace]);
      $conn = \Drupal::service('database');
      $update = $conn->update('tripal_terms_vocabs');
      $update = $update->fields(['idspaces' => serialize($idspaces)]);
      $update = $update->condition('name', $this->getName(), '=');
      $update->execute();
    }
  }
}
