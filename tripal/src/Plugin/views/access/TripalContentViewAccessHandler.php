<?php

namespace Drupal\tripal\Plugin\views\access;

use Drupal\Core\Access\AccessResultInterface;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\Routing\Route;
use Drupal\tripal\Access\TripalEntityAccessControlHandler;
use Drupal\views\Plugin\views\access\AccessPluginBase;
use Drupal\Core\Session\AccountInterface;
use Drupal\views\Attribute\ViewsAccess;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Allows Views to reuse the CRUD permissions for Tripal Content.
 *
 * Note: This also allows them to respect the 'administer tripal content'
 * override.
 */
#[ViewsAccess(
  id: new TranslatableMarkup("tripal_content_views_access"),
  title: new TranslatableMarkup("Use Tripal Content permissions"),
  help: new TranslatableMarkup("Applies the same permissions used for viewing Tripal Content pages based on the types configured."),
)]
class TripalContentViewAccessHandler extends AccessPluginBase {

  use StringTranslationTrait;

  /**
   * {@inheritdoc}
   */
  protected $usesOptions = TRUE;

  /**
   * {@inheritdoc}
   */
  public function buildOptionsForm(&$form, FormStateInterface $form_state) {
    parent::buildOptionsForm($form, $form_state);

    $form['msg'] = [
      [
        '#type' => 'html_tag',
        '#tag' => 'p',
        '#value' => 'Regardless of the settings chosen on this page, the following will remain true:',
      ],
      [
        '#theme' => 'item_list',
        '#items' => [
          "Any user with the 'administer tripal content' permission will be given access to this view.",
          'A user will only see content they have view permission for.',
        ],
      ],
      [
        '#type' => 'html_tag',
        '#tag' => 'br',
      ],
    ];

    // Get all Tripal Content Types.
    $entity_types = \Drupal::service('entity_type.manager')
      ->getStorage('tripal_entity_type')
      ->loadByProperties([]);
    $content_type_options = [];
    foreach ($entity_types as $entity_type) {
      $content_type_options[$entity_type->id()] = $entity_type->getLabel();
    }

    // Add an options for "All" content types to support where the
    // content types change.
    $content_type_options['all'] = $this->t('ALL Existing Content Types');

    $form['content_types'] = [
      '#type' => 'select',
      '#title' => $this->t('Content Types'),
      '#default_value' => $this->options['content_types'],
      '#options' => $content_type_options,
      '#description' => $this->t('Select which content types whose Tripal Content permissions you want to apply to this view.'),
      '#multiple' => TRUE,
      '#required' => TRUE,
    ];

    $form['mode'] = [
      '#type' => 'radios',
      '#title' => $this->t('Multiple Permission Handling'),
      '#default_value' => $this->options['mode'],
      '#description' => $this->t('Indicate how you want multiple content type permissions to be handled.'),
      '#required' => TRUE,
      '#options' => [
        'any' => $this->t('ANY: user has permission to at least one of the content types selected.'),
        'all' => $this->t('ALL: user must have access to all the content types selected.'),
      ],
    ];

    $form['operation'] = [
      '#type' => 'radios',
      '#title' => $this->t('Content Type Permission'),
      '#default_value' => $this->options['operation'],
      '#description' => $this->t('Indicate which of the permissions to use for each content type.'),
      '#required' => TRUE,
      '#options' => [
        'view' => $this->t('VIEW: user has permission to view the selected content type(s).'),
        'create' => $this->t('CREATE: user has permission to view the selected content type(s).'),
        'edit' => $this->t('EDIT: user has permission to edit the selected content type(s).'),
        'unpublish' => $this->t('UNPUBLISH: user has permission to view the selected content type(s).'),
        'delete' => $this->t('DELETE: user has permission to delete the selected content type(s).'),
      ],
    ];

  }

  /**
   * {@inheritdoc}
   */
  public function submitOptionsForm(&$form, FormStateInterface $form_state) {

    $access_options = $form_state->getValue('access_options');
    $selected_content_types = $access_options['content_types'];

    // If someone accidentaly selected 'all' and individual content types,
    // let's just remove 'all' for them.
    if ((count($selected_content_types) > 1) and array_key_exists('all', $selected_content_types)) {
      unset($selected_content_types['all']);
      $access_options['content_types'] = $selected_content_types;
      $form_state->setValue('access_options', $access_options);
      $this->messenger()->addWarning("We detected both 'all' and individual content types selected. As such, we fixed the selection to no longer include 'all'.");
    }

    // Now let's summarize their choice.
    $operation = $access_options['operation'];
    // - Expand mode to fit in the statement.
    $mode_string = 'at least 1 of';
    if ($this->options['mode'] === 'all') {
      $mode_string = 'all of';
    }
    // - if all selected...
    if (array_key_exists('all', $selected_content_types)) {
      $content_types_string = 'the existing content types';
    }
    else {
      $content_types_string = 'the following content types: ' . implode(', ', $selected_content_types);
    }
    // - finally, compile the message.
    $this->messenger()->addStatus("Settings saved successfully. Now a user will only be able to access this view if the have the $operation permission for $mode_string $content_types_string.");
  }

  /**
   * {@inheritdoc}
   *
   * Note: This is called when the view is being rendered. This allows us to
   * catch even programatic rendering or embedding at separate routes.
   */
  public function access(AccountInterface $account) {

    // Let's just use the TripalEntity Access controller directly here.
    return TripalEntityAccessControlHandler::checkManyPermissions(
      $account,
      $this->options['content_types'],
      $this->options['operation'],
      $this->options['mode']
    )->isAllowed();
  }

  /**
   * {@inheritdoc}
   *
   * Note: Altering the route allows Drupal to hide links, etc. before the
   * page is accessed or rendered.
   */
  public function alterRouteDefinition(Route $route) {

    // Add a requirement that points to the correct static method in the
    // TripalEntity access controller.
    $route->setRequirement('_custom_access', static::class . '::checkManyPermissions');
    // Sets the options that this method expects.
    $route->setOption('_content_types', $this->options['content_types']);
    $route->setOption('_comparison_mode', $this->options['mode']);
    $route->setOption('_permission_operation', $this->options['operation']);
  }

  /**
   * Gets parameters from route for use with the TripalEntity Access Controller.
   *
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The user to check permissions for.
   * @param \Symfony\Component\Routing\Route $route
   *   The calling route which gives access to the options.
   *
   * @return \Drupal\Core\Access\AccessResultInterface
   *   The result of this permission check.
   */
  public static function checkManyPermissions(AccountInterface $account, Route $route): AccessResultInterface {

    // Get the options from the route that we set before.
    // @see alterRouteDefinition().
    $content_types = $route->getOption('_content_types');
    $mode = $route->getOption('_comparison_mode');
    $operation = $route->getOption('_permission_operation');

    // Now let's use the TripalEntity Access controller directly here.
    return TripalEntityAccessControlHandler::checkManyPermissions(
      $account,
      $content_types,
      $operation,
      $mode
    );
  }

  /**
   * {@inheritdoc}
   */
  public function summaryTitle() {

    $num_content_types = count($this->options['content_types']);
    $operation = $this->options['operation'];

    // Expand mode to fit in the statement.
    $mode_string = 'at least 1 of';
    if ($this->options['mode'] === 'all') {
      $mode_string = 'all of';
    }

    // If the 'all' content type options was chosen then change the statement.
    if (array_key_exists('all', $this->options['content_types'])) {
      $num_content_types = 'the existing';
    }

    // Return the title displayed in the Views UI.
    // @codingStandardsIgnoreStart
    return $this->t("$operation $mode_string $num_content_types Tripal Content Type(s)");
    // @codingStandardsIgnoreEnd
  }

}
