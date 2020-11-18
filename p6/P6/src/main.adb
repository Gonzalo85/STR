with GNAT.IO;use GNAT.IO;
-- Añadir el paquete controlador
with controlador; use controlador;
-- Añadir el paquete de tiempo real de ADA
with Ada.Real_Time; use Ada.Real_Time;
-- Añadir el paquete calendario de ADA
with Ada.Calendar; use Ada.Calendar;
--Autor:Gonzalo Bueno Rodriguez
--nota sobre put_line y el paso de integer a cadena de caracteres
--al usar Integer'Image(variable_entera) se introduce un blanco delante del campo por lo tanto el formato
--pedido no es posible ya que aparece blancos, se podría evitar pasando de entero a cadena de otra forma

procedure Main is
   -- Definir un tipo puntero a entero
   type puntero is access all Integer;

   -- Definir el tipo tarea MostrarHora con dos parámetros: recibidas (de tipo puntero a entero) y esperadas (de tipo entero)
   task type MostrarHora(recibidas : puntero; esperadas : Integer);
   task body MostrarHora is
      -- Definir variables dia, mes y anio de tipo Day_Number, Month_Number y Year_Number respectivamente
      dia : Day_Number;
      mes : Month_Number;
      anio : Year_Number;
      
      -- Definir la variable segundos de tipo Day_Duration
      segundos : Day_Duration;
      
      -- Definir la variable horas, minutos y segundos_int de tipo entero (segundos_int nos servirá para convertir los segundos a entero y poder trabajar con ellos)
      horas : Integer;
      minutos : Integer;
      segundos_int : Integer;
      
      -- Definir la variable tiempo_calendario de tipo time del paquete calendario. La usaremos para leer la hora del reloj calendario
      tiempo_calendario : Ada.Calendar.Time;
      
      -- Definir la variable sig_instante de tipo time del paquete de tiempo real. La usaremos para los retardos
      sig_instante : Ada.Real_Time.Time;
      
      -- Definir la variable Periodo de tipo Time_Span y asignarle 5 segundos (usar la función Milliseconds)
      periodo : Time_Span := Milliseconds(5000);
   begin
      -- sig_instante=hora actual (se optiene con la función Clock del reloj de tiempo real) + periodo
      sig_instante := Ada.Real_Time.Clock + periodo;
      -- Mientras no se hayan recibido las señales esperadas (como recibidas es un puntero, debe usarse recibidas.all para acceder a su contenido)
      while recibidas.all < esperadas loop
         -- Leer la hora del reloj calendario en tiempo_calendario
         tiempo_calendario := Ada.Calendar.Clock;
         -- Transformar tiempo_calendario a años, meses, días y segundos. Puede usarse el procedimiento Split para hacerlo todo de golpe o las funciones Year, Month, Day y Seconds para hacerlo paso a paso
         Split(tiempo_calendario,anio,mes,dia,segundos);
         
         -- Transformar los segundos a un valor entero. El casting en hada se hace de la siguiente forma: tipo(valor)
         segundos_int := Integer(segundos);
         
         -- Calcular la hora actual como segundos_int/3600
         horas := segundos_int/3600;
         
         -- Eliminar los segundos correspondientes a las horas (horas*3600)
         segundos_int := segundos_int - (horas*3600);
         
         -- Calcular los minutos como segundos_int/60
         minutos := segundos_int/60;
         
         -- Eliminar los segundos correspondientes a los minutos (minutos*60)
         segundos_int := segundos_int - (minutos*60);
         
         -- Mostrar por pantalla dia/mes/año --- horas:minutos:segundos. Usar Put_Line con las siguientes consideraciones:
         -- Para transformar un valor entero a cadena usar Integer'Image(variable_entera)
         -- Para unir dos cadenas de caracteres usar el operador &
         put_line(Integer'Image(dia)&"/\b"&Integer'Image(mes) & "/" & Integer'Image(anio)
                  & "----" & Integer'Image(horas) & ":" & Integer'Image(minutos) & ":" & Integer'Image(segundos_int));  

         -- Esperar hasta sig_instante
         delay until sig_instante;

         -- Calcular el seguiente instante añadiéndole el periodo
         sig_instante := sig_instante + periodo;

      end loop;-- Fin mientras

   end MostrarHora;

   -- Definir el tipo tarea ContarCtrlC con dos parámetros: recibidas (de tipo puntero a entero) y esperadas (de tipo entero)
   task type ContarCtrlC(recibidas : puntero; esperadas : Integer);
   task body ContarCtrlC is
   begin
      -- Mientras no se hayan recibido las señales esperadas (como recibidas es un puntero, debe usarse recibidas.all para acceder a su contenido)
      while recibidas.all < esperadas loop
         -- Como debemos mostrar un mensaje si no se ha pulsado Ctrl+C en 6 segundos, añadir la cláusula Select
         select
            -- Invocar a la entrada Wait de ControladorSigInt para esperar a que se pulse Ctrl+C
            ControladorSigInt.Wait;
            -- Incrementar las señales recibidas
            recibidas.all := recibidas.all + 1;
         -- cláusula or
         or
            -- Retardo relativo de 6.0 segundos
            delay 6.0;
            -- Mostrar por pantalla el mensaje "Han pasado 6 segundos sin recibir una senal"
            Put_Line("Han pasado 6 segundos sin recibir una senial");
       end select;  -- Fin select

    end loop;  -- Fin mientras

   end ContarCtrlC;

   -- Definir el tipo tarea mostrarCtrlC con dos parámetros: recibidas (de tipo puntero a entero) y esperadas (de tipo entero)
   task type mostrarCtrlC(recibidas : puntero; esperadas : Integer);

   task body mostrarCtrlC is
      -- Definir la variable sig_instante de tipo time del paquete de tiempo real. La usaremos para los retardos
      sig_instante : Ada.Real_Time.Time;

      -- Definir la variable Periodo de tipo Time_Span y asignarle 2 segundos (usar la función Milliseconds)
      periodo : Time_Span := Milliseconds(2000);

   begin
      -- sig_instante=hora actual (se optiene con la función Clock del reloj de tiempo real) + periodo
      sig_instante := Ada.Real_Time.Clock + periodo;

      -- Mientras no se hayan recibido las señales esperadas (como recibidas es un puntero, debe usarse recibidas.all para acceder a su contenido)
      while recibidas.all < esperadas loop
         
         -- Mostrar el número de señales recibidas. Usar Put_Line con las siguientes consideraciones:
         -- Para transformar un valor entero a cadena usar Integer'Image(variable_entera)
         -- Para unir dos cadenas de caracteres usar el operador &
         Put_Line("Recibidas: " & Integer'Image(recibidas.all));
         -- Esperar hasta sig_instante
         delay until sig_instante;
         -- Calcular el seguiente instante añadiéndole el periodo
         sig_instante := sig_instante + periodo;
      -- Fin mientras    
      end loop;
      

   end mostrarCtrlC;

   -- Definir la variable esperadas de tipo entero y asignarle un valor
   esperadas : Integer := 5;
   
   -- Definir la variable recibidas de tipo entero permitiendo el acceso a su dirección de memoria (cláusula aliased) e inicializarla a 0
   recibidas : aliased Integer := 0;
   
   -- Definir una tarea de tipo MostrarHora con la dirección de recibidas y la variable esperadas como parámetros
   A : MostrarHora(recibidas'Access,esperadas);
   
   -- Definir una tarea de tipo ContarCtrlC con la dirección de recibidas y la variable esperadas como parámetros
   B : ContarCtrlC(recibidas'Access,esperadas);
   
   
   -- Definir una tarea de tipo mostrarCtrlC con la dirección de recibidas y la variable esperadas como parámetros
   C : mostrarCtrlC(recibidas'Access,esperadas);
begin
   null;
end Main;
