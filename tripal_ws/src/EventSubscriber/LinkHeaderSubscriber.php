<?php

namespace Drupal\tripal_ws\EventSubscriber;

use Symfony\Component\EventDispatcher\EventSubscriberInterface;
use Symfony\Component\HttpKernel\KernelEvents;
use Symfony\Component\HttpKernel\Event\ResponseEvent;

class LinkHeaderSubscriber implements EventSubscriberInterface {

  public static function getSubscribedEvents() {
    return [
      KernelEvents::RESPONSE => 'addLinkHeader',
    ];
  }
  
  /**
   * Adds LINK to the site's response header to point to Tripal Webservices.
   */
  public function addLinkHeader(ResponseEvent $event) {
    global $base_url;
    $api_url = $base_url . '/web-services/';
    
    $response = $event->getResponse();
    // Example: Add a Link header for a stylesheet (adjust as needed)
    $response->headers->set('Link', $api_url, false);
  }
}

// We'll also need this later when actually responding with API requests.
//e.g. make sure response type is correct:
//     drupal_add_http_header('Content-Type', 'application/ld+json');