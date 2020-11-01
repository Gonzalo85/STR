pragma Ada_95;
pragma Source_File_Name (ada_main, Spec_File_Name => "b__main.ads");
pragma Source_File_Name (ada_main, Body_File_Name => "b__main.adb");
pragma Suppress (Overflow_Check);

with System.Restrictions;
with Ada.Exceptions;

package body ada_main is
   pragma Warnings (Off);

   E013 : Short_Integer; pragma Import (Ada, E013, "system__soft_links_E");
   E023 : Short_Integer; pragma Import (Ada, E023, "system__exception_table_E");
   E130 : Short_Integer; pragma Import (Ada, E130, "ada__io_exceptions_E");
   E087 : Short_Integer; pragma Import (Ada, E087, "ada__tags_E");
   E129 : Short_Integer; pragma Import (Ada, E129, "ada__streams_E");
   E052 : Short_Integer; pragma Import (Ada, E052, "interfaces__c_E");
   E025 : Short_Integer; pragma Import (Ada, E025, "system__exceptions_E");
   E132 : Short_Integer; pragma Import (Ada, E132, "system__finalization_root_E");
   E127 : Short_Integer; pragma Import (Ada, E127, "ada__finalization_E");
   E071 : Short_Integer; pragma Import (Ada, E071, "system__task_info_E");
   E138 : Short_Integer; pragma Import (Ada, E138, "system__task_primitives__interrupt_operations_E");
   E017 : Short_Integer; pragma Import (Ada, E017, "system__secondary_stack_E");
   E109 : Short_Integer; pragma Import (Ada, E109, "system__tasking__initialization_E");
   E005 : Short_Integer; pragma Import (Ada, E005, "ada__real_time_E");
   E119 : Short_Integer; pragma Import (Ada, E119, "system__tasking__protected_objects_E");
   E123 : Short_Integer; pragma Import (Ada, E123, "system__tasking__protected_objects__entries_E");
   E117 : Short_Integer; pragma Import (Ada, E117, "system__tasking__queuing_E");
   E146 : Short_Integer; pragma Import (Ada, E146, "system__tasking__stages_E");
   E136 : Short_Integer; pragma Import (Ada, E136, "system__interrupt_management__operations_E");
   E101 : Short_Integer; pragma Import (Ada, E101, "system__interrupts_E");
   E099 : Short_Integer; pragma Import (Ada, E099, "controlador_E");

   Local_Priority_Specific_Dispatching : constant String := "";
   Local_Interrupt_States : constant String := "";

   Is_Elaborated : Boolean := False;

   procedure finalize_library is
   begin
      E099 := E099 - 1;
      declare
         procedure F1;
         pragma Import (Ada, F1, "controlador__finalize_spec");
      begin
         F1;
      end;
      E101 := E101 - 1;
      declare
         procedure F2;
         pragma Import (Ada, F2, "system__interrupts__finalize_spec");
      begin
         F2;
      end;
      E123 := E123 - 1;
      declare
         procedure F3;
         pragma Import (Ada, F3, "system__tasking__protected_objects__entries__finalize_spec");
      begin
         F3;
      end;
      declare
         procedure Reraise_Library_Exception_If_Any;
            pragma Import (Ada, Reraise_Library_Exception_If_Any, "__gnat_reraise_library_exception_if_any");
      begin
         Reraise_Library_Exception_If_Any;
      end;
   end finalize_library;

   procedure adafinal is
      procedure s_stalib_adafinal;
      pragma Import (C, s_stalib_adafinal, "system__standard_library__adafinal");

      procedure Runtime_Finalize;
      pragma Import (C, Runtime_Finalize, "__gnat_runtime_finalize");

   begin
      if not Is_Elaborated then
         return;
      end if;
      Is_Elaborated := False;
      Runtime_Finalize;
      s_stalib_adafinal;
   end adafinal;

   type No_Param_Proc is access procedure;

   procedure adainit is
      Main_Priority : Integer;
      pragma Import (C, Main_Priority, "__gl_main_priority");
      Time_Slice_Value : Integer;
      pragma Import (C, Time_Slice_Value, "__gl_time_slice_val");
      WC_Encoding : Character;
      pragma Import (C, WC_Encoding, "__gl_wc_encoding");
      Locking_Policy : Character;
      pragma Import (C, Locking_Policy, "__gl_locking_policy");
      Queuing_Policy : Character;
      pragma Import (C, Queuing_Policy, "__gl_queuing_policy");
      Task_Dispatching_Policy : Character;
      pragma Import (C, Task_Dispatching_Policy, "__gl_task_dispatching_policy");
      Priority_Specific_Dispatching : System.Address;
      pragma Import (C, Priority_Specific_Dispatching, "__gl_priority_specific_dispatching");
      Num_Specific_Dispatching : Integer;
      pragma Import (C, Num_Specific_Dispatching, "__gl_num_specific_dispatching");
      Main_CPU : Integer;
      pragma Import (C, Main_CPU, "__gl_main_cpu");
      Interrupt_States : System.Address;
      pragma Import (C, Interrupt_States, "__gl_interrupt_states");
      Num_Interrupt_States : Integer;
      pragma Import (C, Num_Interrupt_States, "__gl_num_interrupt_states");
      Unreserve_All_Interrupts : Integer;
      pragma Import (C, Unreserve_All_Interrupts, "__gl_unreserve_all_interrupts");
      Detect_Blocking : Integer;
      pragma Import (C, Detect_Blocking, "__gl_detect_blocking");
      Default_Stack_Size : Integer;
      pragma Import (C, Default_Stack_Size, "__gl_default_stack_size");
      Leap_Seconds_Support : Integer;
      pragma Import (C, Leap_Seconds_Support, "__gl_leap_seconds_support");

      procedure Runtime_Initialize (Install_Handler : Integer);
      pragma Import (C, Runtime_Initialize, "__gnat_runtime_initialize");

      Finalize_Library_Objects : No_Param_Proc;
      pragma Import (C, Finalize_Library_Objects, "__gnat_finalize_library_objects");
   begin
      if Is_Elaborated then
         return;
      end if;
      Is_Elaborated := True;
      Main_Priority := -1;
      Time_Slice_Value := -1;
      WC_Encoding := 'b';
      Locking_Policy := ' ';
      Queuing_Policy := ' ';
      Task_Dispatching_Policy := ' ';
      System.Restrictions.Run_Time_Restrictions :=
        (Set =>
          (False, False, False, False, False, False, False, False, 
           False, False, False, False, False, False, False, False, 
           False, False, False, False, False, False, False, False, 
           False, False, False, False, False, False, False, False, 
           False, False, False, False, False, False, False, False, 
           False, False, False, False, False, False, False, False, 
           False, False, False, False, False, False, False, False, 
           False, False, False, False, False, False, False, False, 
           False, False, False, False, False, False, False, False, 
           True, False, False, False, False, False, False, False, 
           False, False, False, False, False, False),
         Value => (0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
         Violated =>
          (False, False, False, True, True, False, False, False, 
           False, False, False, True, True, True, True, False, 
           True, True, False, True, True, False, True, True, 
           False, True, True, False, True, False, False, True, 
           False, True, False, False, True, False, False, False, 
           True, True, False, False, False, True, True, False, 
           True, True, False, False, True, False, True, False, 
           True, False, True, True, True, False, False, True, 
           False, False, True, False, True, True, False, True, 
           True, True, False, True, False, False, False, True, 
           True, True, True, False, True, False),
         Count => (0, 0, 0, 1, 9, 10, 2, 0, 1, 0),
         Unknown => (False, False, False, False, False, False, True, False, True, False));
      Priority_Specific_Dispatching :=
        Local_Priority_Specific_Dispatching'Address;
      Num_Specific_Dispatching := 0;
      Main_CPU := -1;
      Interrupt_States := Local_Interrupt_States'Address;
      Num_Interrupt_States := 0;
      Unreserve_All_Interrupts := 1;
      Detect_Blocking := 0;
      Default_Stack_Size := -1;
      Leap_Seconds_Support := 0;

      Runtime_Initialize (1);

      Finalize_Library_Objects := finalize_library'access;

      System.Soft_Links'Elab_Spec;
      System.Exception_Table'Elab_Body;
      E023 := E023 + 1;
      Ada.Io_Exceptions'Elab_Spec;
      E130 := E130 + 1;
      Ada.Tags'Elab_Spec;
      Ada.Streams'Elab_Spec;
      E129 := E129 + 1;
      Interfaces.C'Elab_Spec;
      System.Exceptions'Elab_Spec;
      E025 := E025 + 1;
      System.Finalization_Root'Elab_Spec;
      E132 := E132 + 1;
      Ada.Finalization'Elab_Spec;
      E127 := E127 + 1;
      System.Task_Info'Elab_Spec;
      E071 := E071 + 1;
      System.Task_Primitives.Interrupt_Operations'Elab_Body;
      E138 := E138 + 1;
      E052 := E052 + 1;
      Ada.Tags'Elab_Body;
      E087 := E087 + 1;
      System.Soft_Links'Elab_Body;
      E013 := E013 + 1;
      System.Secondary_Stack'Elab_Body;
      E017 := E017 + 1;
      System.Tasking.Initialization'Elab_Body;
      E109 := E109 + 1;
      Ada.Real_Time'Elab_Spec;
      Ada.Real_Time'Elab_Body;
      E005 := E005 + 1;
      System.Tasking.Protected_Objects'Elab_Body;
      E119 := E119 + 1;
      System.Tasking.Protected_Objects.Entries'Elab_Spec;
      E123 := E123 + 1;
      System.Tasking.Queuing'Elab_Body;
      E117 := E117 + 1;
      System.Tasking.Stages'Elab_Body;
      E146 := E146 + 1;
      System.Interrupt_Management.Operations'Elab_Body;
      E136 := E136 + 1;
      System.Interrupts'Elab_Spec;
      System.Interrupts'Elab_Body;
      E101 := E101 + 1;
      controlador'elab_spec;
      E099 := E099 + 1;
   end adainit;

   procedure Ada_Main_Program;
   pragma Import (Ada, Ada_Main_Program, "_ada_main");

   function main
     (argc : Integer;
      argv : System.Address;
      envp : System.Address)
      return Integer
   is
      procedure Initialize (Addr : System.Address);
      pragma Import (C, Initialize, "__gnat_initialize");

      procedure Finalize;
      pragma Import (C, Finalize, "__gnat_finalize");
      SEH : aliased array (1 .. 2) of Integer;

      Ensure_Reference : aliased System.Address := Ada_Main_Program_Name'Address;
      pragma Volatile (Ensure_Reference);

   begin
      gnat_argc := argc;
      gnat_argv := argv;
      gnat_envp := envp;

      Initialize (SEH'Address);
      adainit;
      Ada_Main_Program;
      adafinal;
      Finalize;
      return (gnat_exit_status);
   end;

--  BEGIN Object file/option list
   --   /home/JoseMaria/Compartida/p4/p4_2/obj/controlador.o
   --   /home/JoseMaria/Compartida/p4/p4_2/obj/main.o
   --   -L/home/JoseMaria/Compartida/p4/p4_2/obj/
   --   -L/home/JoseMaria/Compartida/p4/p4_2/obj/
   --   -L/usr/gnat/lib/gcc/x86_64-pc-linux-gnu/4.9.3/adalib/
   --   -static
   --   -lgnarl
   --   -lgnat
   --   -lpthread
   --   -lrt
--  END Object file/option list   

end ada_main;
