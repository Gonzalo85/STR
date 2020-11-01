with GNAT.IO; use GNAT.IO;

package body paquete_buffer is
   -- Definir el cuerpo del objeto protegido Buffer_Acotado_t
   protected body Buffer_Acotado_t is
 
      -- Añadir la la entrada Pon (misma cabecera que en el fichero .ads)
      entry Pon(Item : in Integer)
         -- Añadir la condición de acceso a la entrada (buffer no lleno)
        when Cuenta < Tamanio_Buffer is
      -- Comienzo del código de la entrada Pon
	begin 
         -- Añadir Item a la posición Ultimo de Buffer
         Buffer(Ultimo) := Item;

         -- Mostrar un mensaje indicando que se ha añadido el dato almacenado en la posición Ultimo de Buffer
         put_line("Insertado el Item: " & integer'Image(Buffer(Ultimo)));
		 
         -- Actualizar Ultimo
         Ultimo := Ultimo + 1;
         -- Actualizar Cuenta
         Cuenta := Cuenta + 1;
           
      --Fin del código de la entrada Pon
         end;
      -- Añadir la la entrada Quita (misma cabecera que en el fichero .ads)
      entry Quita(Item : out Integer)
         -- Añadir la condición de acceso a la entrada (buffer no vacío)
        when Cuenta > 0 is
         -- Comienzo del código de la entrada Quita
         begin
         -- Almacenar en Item el contenido de la posición Primero de Buffer
          Item := Buffer(Primero); 
         -- Actualizar Primero
          Primero := Primero + 1;
         -- Actualizar Cuenta
          Cuenta := Cuenta - 1;
		 -- Mostrar un mensaje indicando que se ha leido el dato Item
          put_line("Leyendo el Item: " & integer'Image(Item));

      --Fin del código de la entrada Quita
	  end;
   -- Fin del cuerpo del del objeto protegido Buffer_Acotado_t
   end Buffer_Acotado_t;
end paquete_buffer;
