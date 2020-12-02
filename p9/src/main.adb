-- Establecer la política FIFO con prioridades
pragma Task_Dispatching_Policy(FIFO_Within_Priorities);

with Ada.Command_Line;use Ada.Command_Line;
with Ada.Text_Io; use Ada.Text_Io;
with Ada.Execution_Time; use Ada.Execution_Time;

-- Incluir el paquete de tiempo real
with Ada.Real_Time; use Ada.Real_Time;
-- Incluir el paquete para la asignación de prioridades
with System; use System;
-- Incluir el paquete de identificación de tareas
with Ada.Task_Identification; use Ada.Task_Identification;
--Autor: Gonzalo Bueno Rodriguez

procedure Main is

   -- *************************************************************************
   -- ******************************* MODIFICAR *******************************
   -- *************************************************************************

   -- Definir un tipo PTime de tipo puntero a time
   type PTime is access all Time;

   -- Definir un tipo marco_t de tipo registro con los siguientes campos:
      -- C -> Tiempo de ejecución de la tarea (entero)
      -- T -> Periodo de repetición de la tarea (entero)
      -- P -> Prioridad de la tarea (entero)
      -- id -> identificador de la tarea (entero)
      -- TCritico -> Instante crítico a partir del cual comenzará la acción periódica
   type marco_t is record
      C : Integer;
      T : Integer;
      P : Integer;
      id : Integer;
      TCritico : Time;
   end record;


   -- Definir un tipo Pmarco_t de tipo puntero a marco_t
  type Pmarco_t is access all marco_t;

   type marcos_t is array(integer range <>) of aliased marco_t;
   -- Definir un tipo Pmarcos_t de tipo puntero a marcos_t
   type Pmarcos_t is access marcos_t;

   -- Definir una variable Tareas de tipo Pmarcos_t
   Tareas : Pmarcos_t;


   -- *************************************************************************
   -- *************************** NO MODIFICAR NADA ***************************
   -- *************************************************************************

   procedure spin_m(p: integer) is
      Ti: CPU_Time;
      dif, fin:Time_Span;
      finb:Boolean:=false;
   begin
      fin:=milliseconds(p);
      Ti:=Clock;
      while finb=false loop
         dif:=Clock-Ti;
         if dif>=fin then finb:=True; end if;
      end loop;
   end;

   procedure ObtenerArray(fichero: in String; Tareas: out Pmarcos_t; num_tareas: out Integer) is
      max, j, cont: integer;
      F: Ada.Text_Io.File_Type;
      Linea, dato: String(1..128);
   begin
      Open(F,In_File, fichero);
      Get_Line(F, Linea, max);
      num_tareas:=Integer'Value(Linea(1..max));
      Tareas:=new marcos_t(1..num_tareas);
      for i in 1..num_tareas loop
         Get_Line(F, Linea, max);
         cont:=1;
         j:=1;
         while Linea(cont)/=' ' loop
            dato(j):=Linea(cont);
            j:=j+1;
            cont:=cont+1;
         end loop;
         Tareas(i).C:=integer'Value(dato(1..j-1));
         cont:=cont+1;
         j:=1;
         while Linea(cont)/=' ' loop
            dato(j):=Linea(cont);
            j:=j+1;
            cont:=cont+1;
         end loop;
         Tareas(i).T:=integer'Value(dato(1..j-1));
         cont:=cont+1;
         j:=1;
         while Linea(cont)/=' ' and cont<=max loop
            dato(j):=Linea(cont);
            j:=j+1;
            cont:=cont+1;
         end loop;
         Tareas(i).P:=integer'Value(dato(1..j-1));
         Tareas(i).id:=i;
      end loop;
      for i in 1..num_tareas loop
         Put_Line(integer'Image(tareas(i).id)&" "&integer'Image(Tareas(i).C)&" "&integer'Image(Tareas(i).T)&" "&integer'Image(Tareas(i).P));
      end loop;
   end ObtenerArray;

   -- *************************************************************************
   -- ******************************* MODIFICAR *******************************
   -- *************************************************************************

   -- Definir el tipo tarea TareaMostrarTiempos que tendrá un único parámetro de tipo PTime
   task type TareaMostrarTiempos(param:PTime) is pragma Priority(Max_Priority);
      -- Asignar la prioridad máxima a la tarea TareaMostrarTiempos

   -- Fin de la definición de la tarea TareaMostrarTiempos
   end TareaMostrarTiempos;

   -- Definir el cuerpo de la tarea TareaMostrarTiempos
   task body TareaMostrarTiempos is
      -- Definir una variable sig_inst de tipo time
      sig_inst : Time;
   -- Inicio del cuerpo de la tarea TareaMostrarTiempos
   begin
      -- Asignar a sig_inst la suma del instante crítico (como es un puntero, debemos usar '.all') y 10 milisegundos
      sig_inst := param.all + Milliseconds(10);
      -- Esperar hasta el instante crítico
      delay until param.all;
      -- Para i desde 0 hasta 199
      for i in 0..199 loop
         -- Mostrar el mensaje "********************* x ms *********************", donde x=i*10
         Put_Line("*********************" & Integer'Image(i*10) & "ms *********************");
         -- Esperar hasta el sig_inst
         delay until sig_inst;
         -- Sumar 10 milisegundos a sig_inst
         sig_inst := sig_inst + Milliseconds(10);
      -- Fin para
      end loop;

   -- Fin del cuerpo de TareaMostrarTiempos
   end TareaMostrarTiempos;

   -- Definir el tipo PTareaMostrarTiempos de tipo puntero a TareaMostrarTiempos
   type PTareaMostrarTiempos is access TareaMostrarTiempos;

   -- Definir el tipo tarea TareaPeriodica que tendrá dos parámetros:
   -- marco de tipo Pmarco_t (contiene el marco temporal de la tarea)
   -- tMT de tipo PTareaMostrarTiempos (contiene el identificador de la tarea que muestra los tiempos; lo usaremos para saber cuándo parar)
   task type TareaPeriodica(marco : Pmarco_t; tMT : PTareaMostrarTiempos) is pragma Priority(marco.P);

      -- Asignar la prioridad indicada en el campo P del parámetro marco

   -- Fin de la definición de la tarea TareaPeriodica
   end TareaPeriodica;

   -- Definir el cuerpo de la tarea TareaPeriodica
   task body TareaPeriodica is
      -- Definir una variable sig_inst de tipo time
      sig_inst : Time;
   -- Inicio del cuerpo de la tarea
   begin
      -- Asignar a sig_inst la suma del instante crítico (campo TCritico del parámetro marco) y el periodo de la tarea (campo T del parámetro marco)
      sig_inst := marco.TCritico + Milliseconds(marco.T);
      -- Esperar hasta el instante crítico
      delay until marco.TCritico;
      -- Mientras la tarea tMT no haya terminado (el atributo terminated de una tarea indica si ésta ha terminado o no)
      while tMT'terminated = False loop
         -- Mostrar el mensaje "Tarea id con prioridad P comienza su acción periódica", donde id y P corresponden a los campos id y P del parámetro marco
         Put_Line("Tarea "&Integer'Image(marco.id)&" con prioridad "&Integer'Image(marco.P)&" comienza su accion periodica");
         -- Llamar al procedimiento spin_m usando como parámetro de entrada el campo C del parámetro marco
         spin_m(marco.C);
         -- Mostrar el mensaje "Tarea id termina su acción periódica"
         Put_Line("Tarea "&Integer'Image(marco.id)&" termina su accion periodica");
         -- Esperar hasta sig_inst
         delay until sig_inst;
         -- Sumar el periodo de la tarea a sig_inst
         sig_inst := sig_inst + Milliseconds(marco.T);
      -- Fin bucle mientras
      end loop;
   -- Fin del cuerpo de TareaPeriodica
   end TareaPeriodica;

   -- Definir el tipo PTareaPeriodica de tipo puntero a TareaPeriodica
   type PTareaPeriodica is access TareaPeriodica;

   Procedure LanzarTareas(Tareas: Pmarcos_t; num_tareas: integer) is
   -- Tareas: vector con los marcos temporales de cada tarea
   -- num_tareas: Número de márcos temporales en el vector Tareas


      -- Definir una variable tMT de tipo PTareaMostrarTiempos
   tMT : PTareaMostrarTiempos;
      -- Definir una variable tTP de tipo array desde 1 hasta num_tareas de tipo PTareaPeriodica
   tTP : array(1..num_tareas) of PTareaPeriodica;
      -- Definir una variable TCritico de tipo time permitiendo el acceso a su dirección de memoria
   TCritico : aliased time;
   begin
      -- Guardar en TCritico la hora actual más 2 segundos (2000 milisegundos)
      TCritico := Ada.Real_Time.Clock + Milliseconds(2000);
      -- Crear en tMT una nueva tarea TareaMostrarTiempos pasándole como parámetro la dirección de memoria de TCritico
      tMT := new TareaMostrarTiempos(TCritico'Unchecked_Access);
      -- Como TCritico no es una variable global, debemos usar el atributo Unchecked_Access en luvar de Access

      -- Para i desde 1 hasta num_tareas
      for i in 1..num_tareas loop

         -- Asignar al campo TCritico de la tarea i-ésima el calor de TCritico
         Tareas(i).TCritico := TCritico;
         -- Crear en la posición i-ésima de tTP una nueva tarea TareaPeriodica con la dirección del marco i-ésimo (aquí sí puede usarse access) y tMT
         tTP(i) := new TareaPeriodica(Tareas(i)'Access, tMT);
      -- Fin para
      end loop;
   end;

   -- *************************************************************************
   -- *************************** NO MODIFICAR NADA ***************************
   -- *************************************************************************

   num_tareas:integer;
begin
   if Argument_Count /= 1 then
      Put_Line("El formato correcto es ./main ficheroTareas");
      return;
   end if;
   ObtenerArray(Argument(1), Tareas, num_tareas);
   LanzarTareas(Tareas, num_tareas);
end Main;
