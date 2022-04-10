import os

if __name__=="__main__":

    data_folder = "data"

    folders_to_delete = []
    filenames_map = {}

    files_count = 0

    for root, dirs, files in os.walk(data_folder):
        if (root != data_folder):
            folders_to_delete.append(root)

            for file in files:
                abs_path = os.path.join(root, file)

                filename_split = file.split(".")
            
                original_filename = filename_split[0]
                filename_split[0] = str(files_count)
                files_count += 1

                new_filename = ".".join(filename_split)

                extension = file.split(".")[-1]
                shrinked_name = f"{files_count}.{extension}"

                os.rename(abs_path, os.path.join(data_folder, shrinked_name))
                filenames_map[original_filename] = str(files_count)


    for folder in folders_to_delete:
        os.rmdir(folder)

    for file in os.listdir(data_folder):

        if file.endswith(".html") or file.endswith(".js"):

            with open(os.path.join(data_folder, file), "rt", encoding="utf-8") as f:
                file_content = f.read()

            for original, shrinked in filenames_map.items():
                file_content = file_content.replace(original, shrinked)

            with open(os.path.join(data_folder, file), "wt", encoding="utf-8") as f:
                f.write(file_content)