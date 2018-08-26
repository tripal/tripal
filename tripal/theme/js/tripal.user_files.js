(function ($) {
  
  Drupal.behaviors.TripalUserFiles = {
    attach: function (context, settings) {

      // The Drupal theme_items_list duplicates the classes of the li on
      // the ul of nexted children.  This screws up our collapse/open so
      // we'll remove it.
      $('#tripal-user-file-tree ul').removeAttr('class');
      
      // Set default actions for closed and open folders.
      $('.tree-node-closed').children().hide();
      $('.tree-node-closed').click(function(event) {
        expandNode($(this));
      });
      $('.tree-node-open').click(function(event) {
        collapseNode($(this));
      });
      
      // Keep clicks on the files from propagating up to folders and
      // causing collapse.
      $('.tree-node-file').click(function(event) {
      	event.stopPropagation();
      	
      	// Reset the colors for all of the elements.
        $('li.even').css("background-color", "#EEEEEE");
        $('li.odd').css("background-color", "#FFFFFF");
        
        // Get the file details.
      	showFileDetails($(this));
      	
      	// Higlight the selected file.
      	$(this).css("background-color", "#FFAAAA");
      });
    }
  }

  /**
   * Prints the details of the selected file from the tree.
   */
  function showFileDetails(item) {  
	var fid = item.attr('fid');
	var uid = item.attr('uid');
    $.ajax({
      url : baseurl + '/user/' + uid + '/files/' + fid,
      success: function(data) {
        $('#tripal-user-file-details').html(data);
      }
    });  	
  }
  
  /**
   * Collapses a node in the CV tree browser and removes its children.
   */
  function collapseNode(item) {
    item.removeClass('tree-node-open');
    item.addClass('tree-node-closed');
    item.children().hide()
    item.unbind('click');
    item.click(function(event){
      expandNode($(this));
    })
  }
  
  /**
   * Expands a node in the CV tree browser and loads it's children via AJAX.
   */
  function expandNode(item){
    item.removeClass('tree-node-closed');
    item.addClass('tree-node-open');
    item.children().show()
    item.unbind('click');
    item.click(function(event){
      collapseNode($(this));
    }) 
  }
})(jQuery);
