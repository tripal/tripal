<?php
/**
 * @file
 * Contains \Drupal\tripal\Form\Register.
 */
namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Messenger\MessengerInterface;

/**
 * Provides a test form object.
 */
class Register implements FormInterface {

  /**
   * {@inheritdoc}
   */
  public function getFormID() {
    return 'tripal_admin_form_register';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    $form_data = \Drupal::state()->get('tripal_site_registration', new \Drupal\Core\Form\FormState());

    $form['description'] = [
      '#title' => 'Why Register your Site?',
      '#type' => 'item',
      '#markup' => t('Registering your site is important for continued improvements to the software.  You may opt-in by providing answers to the
        following questions or opt-out by checking the box below. If you opt-in, your site will periodically
        connect to the http://tripal.info website and provide updated registration details. If you opt-out, no
        registration information is communictated. You can opt-out at any time.  If you want previously submitted information
        deleted from the tripal.info database please email <a href = "@admin_url">@admin_text</a>.', ['@admin_url' => 'mailto:admin@tripal.info', '@admin_text' => 'admin@tripal.info']),
      '#allowed_tags' => ['a',],
    ];

    $form['usage_details'] = [
      '#title' => 'How will this data be used?',
      '#type' => 'item',
      '#markup' => t('Tripal is open-source, freely-available, but
        dependent on external funding. When you register your site, it provides important details that can
        be used to justify continued support from funding agencies. The information provided will not be shared publically.
        Information about the Tripal modules installed on your site will be used to help justify continued development to
        funding agencies.  Registration details may be shared with members of Tripal\'s Project Management Committee (PMC) and
        Tripal\'s Steering Committee (TSC) and Tripal extension module usage may be shared with developers
        of the extension modules to aid in their funding requests.'),
    ];

    $form['disable_tripal_reporting'] = [
      '#type' => 'checkbox',
      '#title' => t('Do not register this site (opt-out).'),
      '#default_value' => $form_data->getValue('disable_tripal_reporting'),
      '#description' => "If you do not want to register your site please check
        this box as it will stop the reminder notifications.  You can return later and register at any time.",
      '#ajax' => [
        'callback' => '::_tripal_form_disable_reg_callback',
        'event' => 'click',
        'wrapper' => 'reporting',
      ],
      '#prefix' => '<div id="reporting">',
      '#suffix' => '</div>',
    ];

    $purpose = [0 => t('Production'), 1 => t('Development'), 2 => t('Experimental')];
    $form['details']['tripal_reg_site_purpose'] = [
      '#type' => 'radios',
      '#title' => t('Site Status'),
      '#default_value' => $form_data->getValue('tripal_reg_site_purpose'),
      '#options' => $purpose,
      '#required' => FALSE,
      '#description' => t('Please register your site regardless if it is experimental (to explore tripal),
        for development of a future site (or updates to an existing site), or a site currently
        in production. For funding, it is important to know how many sites are active for each category.  If your site changes
        status, such as from development to production, please remember to return and update the purpose.')
    ];

    $form['details']['tripal_reg_site_modules'] = [
      '#type' => 'checkbox',
      '#default_value' => $form_data->getValue('tripal_reg_site_modules', 1),
      '#title' => t('Report your installed Tripal Extensions.'),
      '#description' => t('When checked, any Tripal extension modules that you have installed will be reported with your site\'s registration information.')
    ];

    $form['details']['tripal_reg_site_description'] = [
      '#type' => 'textarea',
      '#title' => t('Description of the site'),
      '#default_value' => $form_data->getValue('tripal_reg_site_description'),
      '#required' => FALSE,
      '#description' => t('Please provide a brief description of this site.  Consider including
        details such as its purpose, the primary data types your site provides, and the
        research community your site serves.')
    ];

    $form['details']['principal_investigator'] = [
      '#type' => 'fieldset',
      '#title' => t('Principal Investigator Contact Information'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#description' => t('Please provide the name and email of this site\'s principal
       investigator (PI). If the name and email are provided then the PI agrees to
       receive periodic communication from either the Tripal Advisory Committee (TAC) or
       Project Management Committee (PMC) for the purposes of engaging with the larger
       Tripal user community. The PI will NOT be automatically subscribed to mailing lists.')
    ];

    $form['details']['principal_investigator']['principal_investigator_name'] = [
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => $form_data->getValue('principal_investigator_name'),
      '#required' => FALSE,
    ];

    $form['details']['principal_investigator']['principal_investigator_email'] = [
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#default_value' => $form_data->getValue('principal_investigator_email'),
      '#required' => FALSE,
    ];

    $form['details']['tripal_reg_site_admin'] = [
      '#type' => 'fieldset',
      '#title' => t('Site Manager (if different from the principal investigator)'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => t('Please provide the name and email of this site\'s manager if
        different from the PI. Sometimes, site managers desire involvement in community
        activites as well as the PI.  If the name and email are provided then the site manager agrees to
        receive periodic communication from either the Tripal Advisory Committee (TAC) or
        Project Management Committee (PMC) for the purposes of engaging with the larger
        Tripal user community. The site manager will NOT be automatically subscribed to mailing lists.')
    ];

    $form['details']['tripal_reg_site_admin']['tripal_reg_site_admin_name'] = [
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => $form_data->getValue('tripal_reg_site_admin_name'),
      '#required' => FALSE,
    ];

    $form['details']['tripal_reg_site_admin']['tripal_reg_site_admin_email'] = [
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#default_value' => $form_data->getValue('tripal_reg_site_admin_email'),
      '#required' => FALSE,
    ];

    $form['details']['funding'] = [
      '#type' => 'container',
      '#tree' => TRUE,
      '#prefix' => '<div id="funding">',
      '#suffix' => '</div>',
    ];

    $funding_values = $form_data->getValue('funding');
    $default_num_funding = 1;
    if ($funding_values) {
      $default_num_funding = max(count($funding_values), 1);
    }
    $num_funding = $form_state->getValue(['funding', 'num'], $default_num_funding);
    $triggeringElement = $form_state->getTriggeringElement();
    if (!empty($triggeringElement) && ($triggeringElement['#name'] == 'add_funding')) {
      $num_funding++;
    }
    $form_state->setValue(['funding', 'num'], $num_funding);

    $form['details']['funding']['num'] = [
      '#type' => 'hidden',
      '#value' => $num_funding,
    ];

    for ($i = 1; $i <= $num_funding; $i++) {
      $form['details']['funding'][$i] = [
        '#type' => 'fieldset',
        '#title' => t("Funding Source $i"),
        '#tree' => TRUE,
        '#collapsible' => TRUE,
        '#collapsed' => $i !== $num_funding,
        '#description' => t('When requesting funds for additional Tripal development,
          it is important to report the breadth of of funding sources for Tripal sites.
          Please consider sharing this information by providing the granting
          agency, and funding periods.')
      ];

      $form['details']['funding'][$i]['tripal_reg_site_agency'] = [
        '#type' => 'textfield',
        '#default_value' => $form_data->getValue(['funding', $i, 'tripal_reg_site_agency']),
        '#title' => t('Funding Agency'),
      ];

      $form['details']['funding'][$i]['tripal_reg_site_grant'] = [
        '#type' => 'textfield',
        '#default_value' => $form_data->getValue(['funding', $i, 'tripal_reg_site_grant']),
        '#title' => t('Grant Number'),
      ];

      $form['details']['funding'][$i]['tripal_reg_site_amount'] = [
        '#type' => 'textfield',
        '#default_value' => $form_data->getValue(['funding', $i, 'tripal_reg_site_amount']),
        '#title' => t('Funding Amount'),
      ];

      $form['details']['funding'][$i]['funding_period'] = [
        '#type' => 'fieldset',
        '#title' => t('Funding Period'),
        '#tree' => TRUE,
      ];

      $year = (int) date('Y');
      $diff = 20;
      $years = range( $year - $diff, $year + $diff);
      $years = array_combine($years, $years);

      $form['details']['funding'][$i]['funding_period']['tripal_reg_site_start'] = [
        '#type' => 'select',
        '#title' => t('Start Year'),
        '#default_value' => $form_data->getValue(['funding', $i, 'funding_period', 'tripal_reg_site_start'], $year),
        '#options' => $years,
      ];

      $form['details']['funding'][$i]['funding_period']['tripal_reg_site_end'] = [
        '#type' => 'select',
        '#title' => t('End Year'),
        '#default_value' => $form_data->getValue(['funding', $i, 'funding_period', 'tripal_reg_site_end'], $year),
        '#options' => $years,
      ];
    }

    $form['details']['funding']['add_funding'] = [
      '#type' => 'button',
      '#button_type' => 'button',
      '#value' => t('Add additional funding sources'),
      '#href' => '',
      '#name' => 'add_funding',
      '#ajax' => [
        'callback' => '::registerAjaxCallback',
        'wrapper' => 'funding',
      ],
    ];

    // Provide a submit button.
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => !empty($form_data) ? 'Update registration information' : 'Register Site',
    ];
    return $form;
  }

  /**
   * Update the funding sources section.
   *
   * @param array $form
   *   The updated form element.
   * @param FormStateInterface $form_state
   *   The state of the form to be updated.
   *
   * @return array
   *   The desired part of the updated form.
   */
  public function registerAjaxCallback(array &$form, FormStateInterface $form_state) {
    return $form['details']['funding'];
  }

  /**
   * Callback to set the disable_tripal_reporting state.
   *
   * @param array $form
   *   The registration form.
   * @param FormStateInterface $form_state
   *   The state of the registration form.
   */
  public function _tripal_form_disable_reg_callback(array &$form, FormStateInterface $form_state) {
    \Drupal::state()->set('disable_tripal_reporting', (bool) $form_state->getValue('disable_tripal_reporting'));
    return $form['disable_tripal_reporting'];
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    if ($form_state->isSubmitted()) {
      $mail_pi = $form_state->getValue('principal_investigator_email');
      $mail_sa = $form_state->getValue('tripal_reg_site_admin_email');
      if ($form_state->getValue('disable_tripal_reporting') != TRUE) {
        if (!empty($mail_pi) && !\Drupal::service('email.validator')->isValid($mail_pi)) {
          $form_state->setError($form['details']['principal_investigator']['principal_investigator_email'], t('The email address for the principal investigator appears to be invalid.'));
        }
        if (!empty($mail_sa) && !\Drupal::service('email.validator')->isValid($mail_sa)) {
          $form_state->setError($form['details']['site_admin']['tripal_reg_site_admin_email'], t('The email address for the site administrator appears to be invalid.'));
        }
      }

      // Validate funding dates for funding sources we're going to report.
      foreach ($form_state->getValue('funding') as $idx => $funds) {
        if (is_array($funds) and is_int($idx) and !empty($funds['tripal_reg_site_agency']) and !empty($funds['tripal_reg_site_grant'])) {
          if ($funds['funding_period']['tripal_reg_site_start'] > $funds['funding_period']['tripal_reg_site_end']) {
            $form_state->setError($form['details']['funding'][$idx]['funding_period'], t('Please select a valid funding period.'));
          }
        }
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    \Drupal::state()->set('disable_tripal_reporting', TRUE);

    //Check for empty funding periods and remove them.
    $j = 1;
    foreach ($form_state->getValue('funding') as $funding_source) {
      if (is_array($funding_source) && !empty($funding_source['tripal_reg_site_agency']) && !empty($funding_source['tripal_reg_site_grant'])) {
        $form_state->setValue(['fundings', $j], $funding_source);
        $j++;
      }
    }
    $form_state->setValue('funding', $form_state->getValue('fundings', []));
    \Drupal::state()->set('tripal_site_registration', $form_state);

    // Now send the updated info to the Tripal Site.
    // Only register with tripal.info if the user has not opted out.
    $messenger = \Drupal::messenger();
    if ($form_state->getValue('disable_tripal_reporting') == FALSE) {
      $this->tripal_registration_remote_submit($form_state);
      $messenger->addMessage(t('Registration sent to tripal.info'));
      $messenger->addMessage(t('Thank you for registering. You can update your details at any time.'));
    }
    else {
      $messenger->addMessage(t('You are not
        registered with tripal.info. You can change this at any time by
        unchecking the opt out checkbox and submitting the form.'));
    }
  }

  /**
   * Sends registration info to tripal.info
   *
   * @param FormStateInterface $form_data
   *   The form_state of the registration form being submitted.
   *
   * @return mixed
   *   The HTTP response from the remote submission.
   */
  public function tripal_registration_remote_submit(FormStateInterface &$form_data) {
    global $base_url;
    $endpoint = 'http://tripal.info/registration/content/50619jdi8ciayjhygidf';

    $tripal_modules = [];
    // Check if we are reporting Tripal Extensions.
    if ($form_data->getValue('tripal_reg_site_modules', FALSE)) {
      // Get current list of modules.
      $modules = \Drupal::service('extension.list.module')->getList();
      foreach ($modules as $module) {
        // Only want to report non-hidden Tripal Extension modules.
        if (empty($module->info['hidden']) and $module->info['package'] == 'Tripal Extensions') {
          $tripal_modules[$module->info['name']] = [
            'info' => $module->info,
            'status' => $module->status,
          ];
        }
      }
    }

    // Clean up form data
    $outgoing_data['pi_name'] = $form_data->getValue('principal_investigator_name');
    $outgoing_data['pi_email'] = $form_data->getValue('principal_investigator_email');
    $outgoing_data['sa_name'] = $form_data->getValue('tripal_reg_site_admin_name');
    $outgoing_data['sa_email'] = $form_data->getValue('tripal_reg_site_admin_email');
    $outgoing_data['description'] = $form_data->getValue('tripal_reg_site_description');

    foreach ($form_data->getValue('funding') as $idx => $funding_source){
      $outgoing_data['funding_period'][$idx]['funding_agency'] = $funding_source['tripal_reg_site_agency'];
      $outgoing_data['funding_period'][$idx]['funding_grant']  = $funding_source['tripal_reg_site_grant'];
      $outgoing_data['funding_period'][$idx]['funding_start']  = $funding_source['funding_period']['tripal_reg_site_start'] ?? NULL;
      $outgoing_data['funding_period'][$idx]['funding_end']  = $funding_source['funding_period']['tripal_reg_site_end'] ?? NULL;
      $outgoing_data['funding_period'][$idx]['funding_amount']  = $funding_source['tripal_reg_site_amount'];
    }

    $outgoing_data['type'] = $form_data->getValue('tripal_reg_site_purpose');

    // Build the info to send out.
    $outgoing_data['tripal_modules'] = $tripal_modules;
    $outgoing_data['site_name'] = \Drupal::state()->get('site_name', 'Default');
    $outgoing_data['site_url'] = $base_url;

    // Send to tripal.info.
    $client = \Drupal::httpClient();
    $request = $client->post($endpoint, [
      'headers' => [
        'Content-Type' => 'application/json',
        'Accept' => 'application/json',
      ],
      'body' => json_encode($outgoing_data),
    ]);
    $response = json_decode($request->getBody());

    \Drupal::state()->set('tripal_site_registration_last_update', time());

    return $response;
  }

}
