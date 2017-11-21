/* phylotree d3js graphs */

(function ($) {

  var height = 0; // will be dynamically sized

  $(document).ready( function () {

    // Callback function to determine node size.
    var nodeSize = function(d) {
      var size;
      if (d.cvterm_name == "phylo_root") {
        size = treeOptions['root_node_size']; 
      }
      if (d.cvterm_name == "phylo_interior") {
        size = treeOptions['interior_node_size']; 
      }
      if (d.cvterm_name == "phylo_leaf") {
        size = treeOptions['leaf_node_size']; 
      }
      return size;
    }

    // Callback function to determine the node color.
    var organismColor = function(d) {
      var color = null;
      if (d.genus) {
        color = organismColors[d.genus + ' ' + d.species];
      }
      if (color) { 
        return color; 
      }
      else { 
        return 'grey'; 
      }
    };

    // Callback for mouseover event on graph node d.
    var nodeMouseOver = function(d) {
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
    var nodeMouseOut = function(d) {
      var el = $(this);
      el.attr('cursor', 'default');
      var circle = el.find('circle');
      if(!d.children) {
        // restore the color based on organism id for leaf nodes
        circle.attr('fill', organismColor(d));
        var txt = el.find('text');
        txt.attr('font-weight', 'normal');
      }
      else {
        // restore interior nodes to white
        circle.attr('fill', 'white');
      }
    };
    
    // Callback for mousedown/click event on graph node d.
    var nodeMouseDown = function(d) {
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
        // If this node is not associated with a feature but it has an 
        // organism node then this is a taxonomic node and we want to
        // link it to the organism page.
        if (!d.feature_id && d.organism_node_id) {
          window.location.replace(baseurl + '/node/' + d.organism_node_id);
        }
        // leaf node
      }
    };

    // AJAX function for retrieving the tree data.
    $.getJSON(phylotreeDataURL, function(treeData) {
      displayData(treeData);
      $('.phylogram-ajax-loader').hide();
    });

    // Creates the tree using the d3.phylogram.js library.
    function displayData(treeData) {
      height = graphHeight(treeData);
      d3.phylogram.build('#phylogram', treeData, {
        'width' : treeOptions['phylogram_width'],
        'height' : height,
        'fill' : organismColor,
        'size' : nodeSize,
        'nodeMouseOver' : nodeMouseOver,
        'nodeMouseOut' : nodeMouseOut,
        'nodeMouseDown' : nodeMouseDown,
        'skipTicks' : treeOptions['skipTicks']
      });
    }

    /* graphHeight() generate graph height based on leaf nodes */
    function graphHeight(data) {
      function countLeafNodes(node) {
        if(! node.children) {
          return 1;
        }
        var ct = 0;
        node.children.forEach( function(child) {
          ct+= countLeafNodes(child);
        });
        return ct;
      }
      var leafNodeCt = countLeafNodes(data);
      return 22 * leafNodeCt;
    }
  });
})(jQuery);