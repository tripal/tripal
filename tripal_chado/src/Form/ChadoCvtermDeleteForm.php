<?php

namespace Drupal\tripal_chado\Form;

use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\ChadoBuddy\PluginManagers\ChadoBuddyPluginManager;
use Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoCvtermBuddy;

/**
 * This class provides a form for confirming deletion of a chado CV term.
 */
class ChadoCvtermDeleteForm extends FormBase {

  /**
   * Provide the cvterm buddy instance.
   */
  protected ChadoCvtermBuddy $cvterm_buddy;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('tripal_chado.chado_buddy')
    );
  }

  /**
   * Class constructor.
   */
  public function __construct(
    protected readonly ChadoBuddyPluginManager $buddy_manager,
  ) {
    // Create the buddy instances we will need.
    $this->cvterm_buddy = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_cvterm_delete_form';
  }

  /**
   * A simple form for confirming deletion of a chado CV term.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   * @param int $cvterm_id
   *   The cvterm primary key.
   */
  public function buildForm(array $form, FormStateInterface $form_state, $cvterm_id = NULL) {

    // Get values of the CV term.
    $cvterm_records = $this->cvterm_buddy->getCvterm(['cvterm.cvterm_id' => $cvterm_id]);
    if (!$cvterm_records) {
      throw new \Exception("Invalid cvterm_id \"$cvterm_id\" passed to chado_cvterm_delete_form");
    }
    $cvterm_record = reset($cvterm_records);

    $form = [];
    $form['cvterm_id'] = [
      '#type' => 'value',
      '#value' => $cvterm_id,
    ];
    $form['description'] = [
      '#type' => 'table',
      '#rows' => [
        [$this->t('Vocabulary:'), $cvterm_record->getValue('cv.name')],
        [$this->t('Term name:'), $cvterm_record->getValue('cvterm.name')],
        [$this->t('Term definition:'), $cvterm_record->getValue('cvterm.definition')],
        [$this->t('Database:'), $cvterm_record->getValue('db.name')],
        [$this->t('Accession:'), $cvterm_record->getValue('dbxref.accession')],
      ],
    ];
    $form['drop_dbxref'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Delete dbxref'),
      '#description' => $this->t('If checked, also delete the database cross reference record associated with this term.'),
      '#default_value' => 1,
    ];
    $form['sure'] = [
      '#markup' => $this->t('<p><strong>Are you sure you want to delete this term?</strong></p>'),
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => 'Delete',
    ];
    $form['cancel'] = [
      '#type' => 'submit',
      '#value' => 'Cancel',
    ];
    return $form;
  }

  /**
   * Form submit hook for the tripal_chado_cvterm_delete_form form.
   *
   * @param array &$form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cvterm_id = $form_state->getValue('cvterm_id');
    $drop_dbxref = $form_state->getValue('drop_dbxref');
    try {
      $this->cvterm_buddy->deleteCvterm(['cvterm.cvterm_id' => $cvterm_id], ['drop_dbxref' => $drop_dbxref]);
      $this->messenger()->addStatus($this->t('The term has been deleted'));
    }
    catch (ChadoBuddyException $e) {
      $this->messenger()->addError($this->t('Unable to delete the term: @error', ['@error' => $e->getMessage()]));
    }

    // @todo This redirect loses any filters we may have applied.
    $response = new RedirectResponse(Url::fromUserInput('/admin/tripal/loaders/chado_vocabs/chado_cvterms')->toString());
    $response->send();
  }

}
