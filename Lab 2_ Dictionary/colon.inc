extern exit

%define next_entry 0

%macro colon 2
  %ifstr %1
    %ifid %2
      %2:
        dq next_entry
        db %1, 0
        
      %define next_entry %2
    %else
      %fatal "Second argument of a colon macro must be an identifier"
    %endif
  %else
    %fatal "First argument of a colon macro must be a string"
  %endif
%endmacro
