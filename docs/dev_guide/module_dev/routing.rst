
Menus, Links and URLs
============================

Defining URL paths and the programmatic flow to a rendered webpage is known as **Routing**. In Drupal 8, routing is handled by the `Symfony Routing component <http://symfony.com/doc/current/components/routing.html>`_ which replaces ``hook_menu`` in Drupal 7. In your custom module, you will define all static routes in the ``.routing.yml`` file using the `YAML format <https://yaml.org/>`_. The following is an example of a route:

.. code:: YAML

  hello_world.hello:
     path: '/hello/{{name}}'
     defaults:
       _controller:
   '\Drupal\hello_world\Controller\HelloWorldController::helloWorld'
       _title: 'Our first route'
     requirements:
       _permission: 'access content'

This route defines that a user navigating to ``https://yourdrupalsite/hello`` will render the content defined by the ``helloWorld`` method in the ``HelloWorldController`` class. The ``defaults`` key provides parameters to the handlers responsible for returning your content to the user. In this case, that includes the title of the page and where to get the content from. Finally, the ``requirements`` key defines conditions which must be met for the content to display; for example, permissions that the current user must have.

The other key thing to note about the above route is the ``{{name}}`` placeholder used in the path. By surrounding a variable name in curly brackets you can pass parameters to your controller. The important thing to note is that the name of your path variable must match exactly and exist in your specified controller method (i.e. ``HelloWorldController::helloWorld($name)``).


Additional Resources:
 - `Official Drupal Routing documentation <https://www.drupal.org/docs/8/api/routing-system>`_. Tripal uses the default Drupal routing system with no modifications.
 - `Official Drupal Converting hook_menu to Drupal 8 Routing <https://www.drupal.org/docs/8/converting-drupal-7-modules-to-drupal-8/d7-to-d8-upgrade-tutorial-convert-hook_menu-and-hook>`_
 - `BeFused Tutorial: "Introduction to creating menu links in a custom Drupal 8 module" <https://befused.com/drupal/menu-links-custom-module-d8>`_
 - `Appnovation Tutorial: "Drupal 8 Routing: Decoupling hook_menu" <https://www.appnovation.com/blog/drupal-8-routing-decoupling-hookmenu>`_

Menu Items
-------------

The menu system has an extensive user interface (UI) for defining menus and the links within them. This is great for management for your Tripal site as it allows you to dynamically add menu items requested by your community. However, when developing custom modules, you will also want to define these menu items programmatically to save time and provide navigation to other sites using your module. To do this you will want to use the ``.links.menu.yml`` file which lives in the base directory of your module. It looks like this:

.. code::

  hello_world.hello:
    title: 'Hello'
    description: 'Get your dynamic salutation.'
    route_name: hello_world.hello
    menu_name: main
    weight: 0

This defines a typically menu link; in this case, a link labelled "Hello" will appear at the top level of the main navigational menu. The machine name of the menu can be found in the path when adding links through the UI. In addition to the basic menu link demonstrated above there are also:

 - local tasks: tabs at the top of the page linking to different sub-pages.
 - local action: link at the top of a page (i.e. "+ Add Content" on ``admin/content``) which allows the admin to complete an action.
 - contextual links: similar to tabs but appear near the title (e.g. view, edit on a content page). These are different from tabs because they are dynamic and often require a parameters (e.g. entity id).

Check out the additional resources for how to define these other types of menu items and for more information in general.

Additional Resources
 - `Official Drupal 8 Menu API docs <https://www.drupal.org/docs/8/api/menu-api>`_
 - `What is a menu? <https://www.drupal.org/docs/user_guide/en/menu-concept.html>`_
 - `Official Drupal docs: Add a menu link <https://www.drupal.org/docs/8/creating-custom-modules/add-a-menu-link>`_
 - `BeFused Tutorial: Introduction to creating menu links in a custom Drupal 8 module <https://befused.com/drupal/menu-links-custom-module-d8>`_

Links
------

When programmatically creating page content, you will often want to add links. To add internal links, use the route name as shown below. This ensures that your link doesn't break if the route is changed.

.. code:: php

  use Drupal\Core\Link;
  $link = Link::createFromRoute('This is a link', 'entity.node.canonical', ['node' => 1]);

To generate a link to an external resource, you can use the following:

.. code:: php

  $link = Link::fromTextAndUrl('This is a link',
    Url::fromUri('http://www.google.com'));

For all the different ways to generate URLs see the following resources -the tutorial is particularly complete.

Additional Resources
 - `Agaric Tutorial: Creating Links in Code for Drupal 8 <https://agaric.coop/blog/creating-links-code-drupal-8>`_
 - `Official Drupal docs: How to upgrade links from Drupal 7 <https://www.drupal.org/node/2346779>`_
