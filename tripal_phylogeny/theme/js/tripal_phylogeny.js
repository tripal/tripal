/* phylotree d3js graphs */

(function ($) {

  var height = 0; // will be dynamically sized

  $(document).ready( function () {

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

    // function to generate color based on the organism genus and species
    // on graph node d
    var organismColor = function(d) {
      var color = null;
      if (d.fo_genus) {
        color = organismColors[d.fo_genus + ' ' + d.fo_species];
      }
      if (color) { return color; }
      else { return 'grey'; }
    };

    // callback for mouseover event on graph node d
    var nodeMouseOver = function(d) {
      var el =$(this);
      el.attr('cursor', 'pointer');
      var circle = el.find('circle');
      // highlight in yellow no matter if leaf or interior node
      circle.attr('fill', 'yellow');
      if(! d.children) {
        // only leaf nodes have descriptive text
        var txt = el.find('text');
        txt.attr('font-weight', 'bold');
      }
    };
    
    // callback for mouseout event on graph node d
    var nodeMouseOut = function(d) {
      var el = $(this);
      el.attr('cursor', 'default');
      var circle = el.find('circle');
      if(! d.children) {
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
    
    // callback for mousedown/click event on graph node d
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
        // leaf node
      }
    };

    $.getJSON(phylotreeDataURL, function(treeData) {
      displayData(treeData);
      $('.phylogram-ajax-loader').remove();
    });

    function displayData(treeData) {
      height = graphHeight(treeData);
      d3.phylogram.build('#phylogram', treeData, {
        'width' : treeOptions['phylogram_width'],
        'height' : height,
        'fill' : organismColor,
        'size' : nodeSize,
        'nodeMouseOver' : nodeMouseOver,
        'nodeMouseOut' : nodeMouseOut,
        'nodeMouseDown' : nodeMouseDown
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
