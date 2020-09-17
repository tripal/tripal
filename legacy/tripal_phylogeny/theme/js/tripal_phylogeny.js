/* phylotree d3js graphs */

(function ($) {

  "use strict";

  // Will be dynamically sized.
  var height = 0;

  // Store our function as a property of Drupal.behaviors.
  Drupal.behaviors.TripalPhylotree = {
    attach: function (context, settings) {

      // Retrieve the data for this tree.
      var data_url = Drupal.settings.tripal_chado.phylotree_url;
      $.getJSON(data_url, function(treeData) {
        phylogeny_display_data(treeData);
        $('.phylogram-ajax-loader').hide();
      });
    }
  }

  // Callback function to determine node size.
  var phylogeny_node_size = function(d) {
    var size;
    var tree_options = Drupal.settings.tripal_chado.tree_options;
    if (d.cvterm_name == "phylo_root") {
      size = tree_options['root_node_size'];
    }
    if (d.cvterm_name == "phylo_interior") {
      size = tree_options['interior_node_size'];
    }
    if (d.cvterm_name == "phylo_leaf") {
      size = tree_options['leaf_node_size'];
    }
    return size;
  }

  // Callback function to determine the node color.
  var phylogeny_organism_color = function(d) {
    var organism_color = Drupal.settings.tripal_chado.org_colors;
    var color = null;

    if (d.fo_genus) {
      color = organism_color[d.fo_organism_id];
    }
    if (color) {
      return color;
    }
    else {
      return 'grey';
    }
  };

  // Callback for mouseover event on graph node d.
  var phylogeny_node_mouse_over = function(d) {
    var el = $(this);
    el.attr('cursor', 'pointer');
    var circle = el.find('circle');
    // highlight in yellow no matter if leaf or interior node
    circle.attr('fill', 'yellow');
    if(!d.children) {
      // only leaf nodes have descriptive text
      var txt = el.find('text');
      txt.attr('font-weight', 'bold');
    }
  };

  // Callback for mouseout event on graph node d.
  var phylogeny_node_mouse_out = function(d) {
    var el = $(this);
    el.attr('cursor', 'default');
    var circle = el.find('circle');
    if(!d.children) {
      // restore the color based on organism id for leaf nodes
      circle.attr('fill', phylogeny_organism_color(d));
      var txt = el.find('text');
      txt.attr('font-weight', 'normal');
    }
    else {
      // restore interior nodes to white
      circle.attr('fill', 'white');
    }
  };

  // Callback for mousedown/click event on graph node d.
  var phylogeny_node_mouse_down = function(d) {
    var el = $(this);
    var title = (! d.children ) ? d.name : 'interior node ' + d.phylonode_id;

    if(d.children) {
      // interior node
      if(d.phylonode_id) {
      }
      else {
        // this shouldn't happen but ok
      }
    }
    else {
      if(d.feature_eid) {
        window.location.href = baseurl + '/bio_data/' + d.feature_eid;

        return;
      }
      // If this node is not associated with a feature but it has an
      // organism node then this is a taxonomic node and we want to
      // link it to the organism page.
      if (!d.feature_id && d.organism_nid) {
        window.location.replace(baseurl + '/node/' + d.organism_nid);
      }
      if (!d.feature_id && d.organism_eid) {
        window.location.replace(baseurl + '/bio_data/' + d.organism_eid);
      }
      // leaf node
    }
  };

  // Creates the tree using the d3.phylogram.js library.
  function phylogeny_display_data(treeData) {
    var height = phylogeny_graph_height(treeData);
    var tree_options = Drupal.settings.tripal_chado.tree_options;
    d3.phylogram.build('#phylogram', treeData, {
      'width' : tree_options['phylogram_width'],
      'height' : height,
      'fill' : phylogeny_organism_color,
      'size' : phylogeny_node_size,
      'nodeMouseOver' : phylogeny_node_mouse_over,
      'nodeMouseOut' : phylogeny_node_mouse_out,
      'nodeMouseDown' : phylogeny_node_mouse_down,
      'skipTicks' : tree_options['skipTicks']
    });
  }

  /* graphHeight() generate graph height based on leaf nodes */
  function phylogeny_graph_height(data) {
    function count_leaf_nodes(node) {
      if(! node.children) {
        return 1;
      }
      var ct = 0;
      node.children.forEach( function(child) {
        ct+= count_leaf_nodes(child);
      });
      return ct;
    }
    var leafNodeCt = count_leaf_nodes(data);
    return 22 * leafNodeCt;
  }
})(jQuery);
