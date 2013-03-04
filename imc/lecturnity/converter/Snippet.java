package imc.lecturnity.converter;

public class Snippet
{
   public String keyword;
   public String replace;

   Snippet(String keyword, String replace)
   {
      this.keyword = "%" + keyword + "%";
      this.replace = replace;
   }
}
